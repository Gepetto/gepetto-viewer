// Copyright (c) 2015-2018, LAAS-CNRS
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of gepetto-viewer.
// gepetto-viewer is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// gepetto-viewer is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer. If not, see <http://www.gnu.org/licenses/>.

#include "gepetto/gui/pythonwidget.hh"

#include <QAction>
#include <QFileDialog>
#include <QSettings>
#include <PythonQt.h>
#include <gui/PythonQtScriptingConsole.h>
#include <PythonQtClassInfo.h>
#include <PythonQt_QtBindings.h>

#include <boost/python.hpp>

#include "gepetto/gui/osgwidget.hh"
#include "gepetto/gui/mainwindow.hh"
#include "gepetto/gui/plugin-interface.hh"

#include "../../src/gui/python-decorator.hh"
#include "../../src/gui/python-bindings.hh"

namespace bp = boost::python;

BOOST_PYTHON_MODULE(gepettogui_internals)
{
  exposeOSG();
  exposeGV();
  exposeGG();
}

namespace gepetto {
    typedef gui::MainWindow GMainWindow;

    namespace gui {
      namespace {
        void addSignalHandler (PythonQtObjectPtr obj, const QString& callable,
            QObject* sender, const char* signal) {
          PythonQt* pqt = PythonQt::self();
          PythonQtObjectPtr call = pqt->lookupCallable(obj, callable);
          if (call.isNull()) {
            qDebug() << "Callable" << callable << "not found.";
            return;
          }
          if (!pqt->addSignalHandler(sender, signal, call)) {
            qDebug() << "Signal" << signal << "not found in object"
              << sender->objectName();
          } else {
            qDebug() << "Connected"
              << signal << "of" << sender->objectName()
              << "to" << callable;
          }
        }

        const QString var = "pluginInstance";
      }

      PythonWidget::PythonWidget(QWidget *parent) :
        QDockWidget("&PythonQt console", parent)
      {
        PyImport_AppendInittab("gepettogui_internals",
#if PY_MAJOR_VERSION==3
            &PyInit_gepettogui_internals
#elif PY_MAJOR_VERSION==2
            &initgepettogui_internals
#endif
            );

        setObjectName ("gepetto-gui.pythonqtconsole");
        PythonQt::init(PythonQt::RedirectStdOut);
        PythonQt_init_QtBindings();
        PythonQtObjectPtr mainContext = PythonQt::self()->getMainModule();
        PythonQtObjectPtr sys = PythonQt::self()->importModule ("sys");
        sys.evalScript ("argv = ['gepetto-gui']");
        mainContext.evalScript ("import PythonQt");
        console_ = new PythonQtScriptingConsole(NULL, mainContext);

        PythonQt::self()->addDecorators (new PythonDecorator());

        PythonQt::self()->registerQObjectClassNames(QStringList()
            << "BodyTreeWidget"
            << "BodyTreeItem"
            << "SelectionEvent"
            << "MainWindow");

        PythonQt::self()->registerCPPClass ("MainWindow", "QMainWindow", "gepetto");
        PythonQt::self()->registerCPPClass ("OSGWidget" , "QWidget"    , "gepetto");

        console_->QTextEdit::clear();
        console_->consoleMessage(
            "PythonQt command prompt\n"
            "Use Shift+Enter for multiline code.\n"
            );
        console_->appendCommandPrompt();

        QWidget* widget = new QWidget;
        QVBoxLayout* layout = new QVBoxLayout;
        button_ = new QPushButton;

        button_->setText("Choose file");
        layout->addWidget(console_);
        layout->addWidget(button_);
        widget->setLayout(layout);
        this->setWidget(widget);

        toggleViewAction()->setShortcut(gepetto::gui::DockKeyShortcutBase + Qt::Key_A);
        connect(button_, SIGNAL(clicked()), SLOT(browseFile()));

        bp::import ("gepettogui_internals");
      }

      PythonWidget::~PythonWidget()
      {
        foreach (const PythonQtObjectPtr& m, modules_)
          unloadModulePlugin(m);
	PythonQt::cleanup();
      }

      void PythonWidget::browseFile()
      {
        QFileDialog* fd = new QFileDialog;

        fd->setFileMode(QFileDialog::ExistingFile);
        fd->setNameFilter("All python file (*.py)");
        if (fd->exec() == QDialog::Accepted) {
          QStringList file = fd->selectedFiles();

          PythonQtObjectPtr mainContext = PythonQt::self()->getMainModule();
          mainContext.evalFile(file.at(0));
        }
        fd->close();
        fd->deleteLater();
      }

      void PythonWidget::saveHistory (QSettings& settings)
      {
        settings.beginGroup("pythonqt");
        QStringList history = console_->history ();
        int limit = 200;
        int start = std::max(history.length() - limit, 0);
        QList<QVariant> h;
        foreach (QString s, history.mid(start)) {
          h << s;
        }
        settings.setValue("history", h);
        settings.endGroup();
      }

      void PythonWidget::restoreHistory (QSettings& settings)
      {
        settings.beginGroup("pythonqt");
        QList<QVariant> h = settings.value("history").toList();
        QStringList history;
        foreach(QVariant v, h) {
          history << v.toString();
        }
        console_->setHistory (history);
        settings.endGroup();
      }

      bool PythonWidget::hasPlugin (const QString& name)
      {
        return modules_.contains (name);
      }

      void PythonWidget::loadScriptPlugin(QString moduleName, QString fileName)
      {
        PythonQt* pqt = PythonQt::self();
        PythonQtObjectPtr module = pqt->createModuleFromFile (moduleName, fileName);
        if (pqt->handleError()) {
          pqt->clearError();
          return;
        }
        if (module.isNull()) {
          qDebug() << "Enable to load module" << moduleName << "from script"
            << fileName;
          return;
        }
        loadPlugin (moduleName, module);
      }

      void PythonWidget::runScript(QString fileName)
      {
        PythonQt* pqt = PythonQt::self();
        PythonQtObjectPtr mainContext = pqt->getMainModule();
        mainContext.evalFile(fileName);

        if (pqt->handleError()) {
          pqt->clearError();
          qDebug() << "Failed to run script" << fileName;
        }
      }

      void PythonWidget::loadModulePlugin(QString moduleName)
      {
        PythonQt* pqt = PythonQt::self();
        PythonQtObjectPtr module = pqt->importModule (moduleName);
        if (pqt->handleError()) {
          pqt->clearError();
          return;
        }
        if (module.isNull()) {
          qDebug() << "Unable to load module" << moduleName;
          return;
        }
        loadPlugin (moduleName, module);
      }

      void PythonWidget::loadPlugin(QString moduleName, PythonQtObjectPtr module)
      {
        PythonQt* pqt = PythonQt::self();
        MainWindow* main = MainWindow::instance();
        module.addObject("windowsManager", main->osg().get());

        QVariantList args; args << QVariant::fromValue((QObject*)main);
        QVariant instance = module.call("Plugin", args);
        module.addVariable(var, instance);

        QDockWidget* dw = qobject_cast<QDockWidget*>(instance.value<QObject*>());
        if (dw) main->insertDockWidget(dw, Qt::RightDockWidgetArea);
        // PythonQtObjectPtr dockPyObj (instance);
        PythonQtObjectPtr dockPyObj = pqt->lookupObject(module,var);
        addSignalHandlersToPlugin(dockPyObj);
        modules_[moduleName] = module;
      }

      void PythonWidget::unloadModulePlugin(QString moduleName)
      {
        if (modules_.contains(moduleName)) {
          PythonQtObjectPtr module = modules_.value(moduleName);
          unloadModulePlugin(module);
          modules_.remove(moduleName);
        }
      }

      void PythonWidget::unloadModulePlugin(PythonQtObjectPtr module )
      {
        PythonQt* pqt = PythonQt::self();
        QVariant instance = pqt->getVariable(module, var);
        QDockWidget* dw = qobject_cast<QDockWidget*>(instance.value<QObject*>());
        if (dw) MainWindow::instance()->removeDockWidget(dw);
        module.removeVariable (var);
      }

      void PythonWidget::addToContext(QString const& name, QObject* obj)
      {
        PythonQtObjectPtr mainContext = PythonQt::self()->getMainModule();
        mainContext.addObject(name, obj);
      }

      void PythonWidget::addSignalHandlersToPlugin(PythonQtObjectPtr plugin)
      {
        MainWindow* main = MainWindow::instance();
        addSignalHandler(plugin, "osgWidget",
            main, SIGNAL(viewCreated(OSGWidget*)));
        QAction* reconnect = main->findChild<QAction*>("actionReconnect");
        if (reconnect)
          addSignalHandler(plugin, "resetConnection",
              reconnect, SIGNAL(triggered()));
        else
          qDebug() << "Could not find actionReconnect button. The plugin will"
            << "not be able to reset CORBA connections";
        QAction* refresh = main->findChild<QAction*>("actionRefresh");
        if (refresh)
          addSignalHandler(plugin, "refreshInterface",
              refresh, SIGNAL(triggered()));
        else
          qDebug() << "Could not find actionRefresh button. The plugin will"
            << "not be able to refresh interface.";
      }

      QVariantList PythonWidget::callPluginMethod (const QString& method,
          const QVariantList& args,
          const QVariantMap& kwargs) const
      {
        PythonQt* pqt = PythonQt::self();
        QVariantList ret;
        foreach (const PythonQtObjectPtr& m, modules_)
        {
          PythonQtObjectPtr dockPyObj = pqt->lookupObject(m,var);
          PythonQtObjectPtr call = pqt->lookupCallable(dockPyObj, method);
          if (!call.isNull()) {
            ret << call.call(args, kwargs);
          }
        }
        return ret;
      }
    }
}
