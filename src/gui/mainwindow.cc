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

#include "gepetto/gui/mainwindow.hh"
#include "ui_mainwindow.h"

#include <sstream>

#include <QtGlobal>
#include <QScrollBar>
#include <QMessageBox>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
# include <QtConcurrent>
#endif

#include <osg/Version>

#include "gepetto/gui/windows-manager.hh"
#include "gepetto/gui/osgwidget.hh"
#include "gepetto/gui/tree-item.hh"
#include "gepetto/gui/shortcut-factory.hh"
#include "gepetto/gui/dialog/dialogloadrobot.hh"
#include "gepetto/gui/dialog/dialogloadenvironment.hh"
#include "gepetto/gui/plugin-interface.hh"
#include "gepetto/gui/selection-handler.hh"
#include "gepetto/gui/action-search-bar.hh"
#include "gepetto/gui/node-action.hh"

#include <gepetto/gui/config-dep.hh>

#if GEPETTO_GUI_HAS_PYTHONQT
# include <gepetto/gui/pythonwidget.hh>
#endif

namespace gepetto {
  namespace gui {
    MainWindow* MainWindow::instance_ = NULL;

    MainWindow::MainWindow(Settings* settings, QWidget *parent) :
      QMainWindow(parent),
      settings_ (settings),
      ui_(new ::Ui::MainWindow),
      centralWidget_ (),
      osgViewerManagers_ (),
      worker_ (),
      actionSearchBar_ (new ActionSearchBar(this))
    {
      MainWindow::instance_ = this;
      ui_->setupUi(this);

      // Setup the body tree view
      osgViewerManagers_ = WindowsManager::create(ui_->bodyTreeContent);
      ui_->bodyTreeContent->init(ui_->bodyTree, ui_->propertyArea);

      // This scene contains elements required for User Interaction.
      osg()->createScene("gepetto-gui");
      // TODO remove me. This is kept for backward compatibility
      osg()->createScene("hpp-gui");

      // Setup the main OSG widget
      connect (ui_->actionRefresh, SIGNAL (triggered()), SLOT (requestRefresh()));

      collisionLabel_ = new QLabel("No collisions.");
      shortcutFactory_ = new ShortcutFactory;
#if GEPETTO_GUI_HAS_PYTHONQT
      pythonWidget_ = new PythonWidget(this);
#endif
      setupInterface();
      connect(ui_->actionChange_shortcut, SIGNAL(triggered()), shortcutFactory_, SLOT(open()));

      selectionHandler_ = new SelectionHandler(osgViewerManagers_);
      selectionHandler_->addMode(new MultiSelection(osgViewerManagers_));
      selectionHandler_->addMode(new UniqueSelection(osgViewerManagers_));

      ui_->mainToolBar->addWidget(selectionHandler_);
    }

    MainWindow::~MainWindow()
    {
      delete shortcutFactory_;
#if GEPETTO_GUI_HAS_PYTHONQT
      removeDockWidget(pythonWidget_);
      delete pythonWidget_;
#endif
      pluginManager()->clearPlugins();
      osgViewerManagers_.reset();
      worker_.quit();
      worker_.wait();
      delete ui_;
    }

    MainWindow *MainWindow::instance()
    {
      return instance_;
    }

    void MainWindow::insertDockWidget(QDockWidget *dock, Qt::DockWidgetArea area, Qt::Orientation orientation)
    {
      addDockWidget(area, dock, orientation);
      dock->setVisible (false);
      dock->toggleViewAction ()->setIcon(QIcon::fromTheme("window-new"));
      dock->adjustSize();
      ui_->menuWindow->addAction(dock->toggleViewAction ());
      actionSearchBar_->addAction(dock->toggleViewAction ());
      connect(dock,SIGNAL(visibilityChanged(bool)),SLOT(dockVisibilityChanged(bool)));
    }

    void MainWindow::removeDockWidget(QDockWidget *dock)
    {
      ui_->menuWindow->removeAction(dock->toggleViewAction());
      disconnect(dock);
      QMainWindow::removeDockWidget(dock);
    }

    WindowsManagerPtr_t MainWindow::osg() const
    {
      return osgViewerManagers_;
    }

    BodyTreeWidget *MainWindow::bodyTree() const
    {
      return ui_->bodyTreeContent;
    }

    QList<OSGWidget *> MainWindow::osgWindows() const
    {
      return osgWindows_;
    }

    PluginManager *MainWindow::pluginManager()
    {
      return &(settings_->pluginManager_);
    }

    ActionSearchBar* MainWindow::actionSearchBar () const
    {
      return actionSearchBar_;
    }

    void MainWindow::log(const QString &text)
    {
      if (thread() != QThread::currentThread()) {
        emit logString(text);
        return;
      }
      ui_->logText->insertHtml("<hr/><font color=black>"+text+"</font>");
    }

    void MainWindow::logError(const QString &text)
    {
      if (thread() != QThread::currentThread()) {
        emit logErrorString(text);
        return;
      }
      if (!ui_->dockWidget_log->isVisible()) {
        ui_->dockWidget_log->show();
      }
      QScrollBar* sb = ui_->logText->verticalScrollBar();
      bool SBwasAtBottom = sb->value() == sb->maximum();
      ui_->logText->insertHtml("<hr/><font color=red>"+text+"</font>");
      if (SBwasAtBottom)
        sb->setValue(sb->maximum());
    }

    QMenu *MainWindow::pluginMenu() const
    {
      return ui_->menuWindow;
    }

    void MainWindow::logJobStarted(int id, const QString &text)
    {
      emit logString (QString ("Starting job ") + QString::number (id) + ": " + text);
    }

    void MainWindow::logJobDone(int id, const QString &text)
    {
      emit logString (QString ("Job ") + QString::number (id) + " done: " + text);
    }

    void MainWindow::logJobFailed(int id, const QString &text)
    {
      emit logErrorString (QString ("Job ") + QString::number (id) + " failed: " + text);
    }

    OSGWidget *MainWindow::createView(const std::string& name)
    {
      if (thread() != QThread::currentThread()) {
        qDebug() << "createView must be called from the main thread.";
        throw std::runtime_error("Cannot create a new window.");
      }
      OSGWidget* osgWidget = new OSGWidget (osgViewerManagers_, name, this, 0
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
          , osgViewer::Viewer::SingleThreaded
#endif
          );
      osgWidget->setObjectName(name.c_str());
      addOSGWidget (osgWidget);
      emit viewCreated(osgWidget);
      return osgWidget;
    }

    void MainWindow::requestRefresh()
    {
      emit refresh ();
    }

    void MainWindow::createDefaultView()
    {
      std::stringstream ss; ss << "window_" << osgWindows_.size();
      createView (ss.str());
    }

    void MainWindow::addOSGWidget(OSGWidget* osgWidget)
    {
      QDockWidget* dockOSG = new QDockWidget (
          tr("Window ") + osgWidget->objectName(), this);
      dockOSG->setObjectName ("gepetto-gui.osg." + osgWidget->objectName());
      dockOSG->setWidget(osgWidget);
      // TODO at the moment, when the widget is made floating and then non-floating
      // again, the OSGWidget becomes hidden. I could not find the bug so I removed
      // the feature DockWidgetFloatable.
      dockOSG->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
      connect(dockOSG,SIGNAL(visibilityChanged(bool)),SLOT(dockVisibilityChanged(bool)));
      addDockWidget(Qt::RightDockWidgetArea, dockOSG);
      if (osgWindows_.empty()) {
        // This OSGWidget should be the central view
        centralWidget_ = osgWidget;
        osg()->addSceneToWindow("gepetto-gui", centralWidget_->windowID());
        // TODO remove me. This is kept for backward compatibility
        osg()->addSceneToWindow("hpp-gui", centralWidget_->windowID());
      }
      actionSearchBar_->addAction(new NodeAction("Attach camera " + osgWidget->objectName() + " to selected node", osgWidget, this));
      osgWidget->addAction(actionSearchBar_->showAction());
      osgWindows_.append(osgWidget);
      settings_->restoreDockWidgetsState ();
      // When creating a window from Python, it isn't desirable to create a
      // hidden window. We overwrite the visibility.
      dockOSG->show ();

      // Add the widget to the window list.
      QMenu* views3D = ui_->menuWindow->findChild<QMenu*>("3d views"
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
              , Qt::FindDirectChildrenOnly
#endif
              );
      views3D->addAction(dockOSG->toggleViewAction());
    }

    void MainWindow::openLoadRobotDialog()
    {
      statusBar()->showMessage("Loading a robot...");
      DialogLoadRobot* d = new DialogLoadRobot (this);
      if (d->exec () == QDialog::Accepted) {
        createCentralWidget();
        DialogLoadRobot::RobotDefinition rd = d->getSelectedRobotDescription();

        QString urdfFile = QString("package://%1/urdf/%2%3.urdf").arg(rd.package_).arg(rd.modelName_).arg(rd.urdfSuf_);
        try {
          osgViewerManagers_->addURDF(rd.robotName_.toStdString(), urdfFile.toStdString());
          osgViewerManagers_->addSceneToWindow(rd.robotName_.toStdString(), centralWidget_->windowID());
        } catch (std::runtime_error& exc) {
          logError (exc.what ());
        }
        robotNames_.append (rd.robotName_);

        QString what = QString ("Loading robot ") + rd.name_;
        foreach (ModelInterface* loader, pluginManager()->get <ModelInterface> ()) {
          QtConcurrent::run (loader, &ModelInterface::loadRobotModel, rd);
          logString (what);
        }
      }
      d->close();
      statusBar()->clearMessage();
      d->deleteLater();
    }

    void MainWindow::openLoadEnvironmentDialog()
    {
      statusBar()->showMessage("Loading an environment...");
      DialogLoadEnvironment* e = new DialogLoadEnvironment (this);
      if (e->exec() == QDialog::Accepted) {
        createCentralWidget();
        DialogLoadEnvironment::EnvironmentDefinition ed = e->getSelectedDescription();

        QString urdfFile = QString("package://%1/urdf/%2.urdf").arg(ed.package_).arg(ed.urdfFilename_);
        try {
          osgViewerManagers_->addUrdfObjects(ed.envName_.toStdString(),
                                             urdfFile   .toStdString(),
                                             true);
          osgViewerManagers_->addSceneToWindow(ed.envName_.toStdString(),
                                               centralWidget_->windowID());
        } catch (std::runtime_error& exc) {
          log (exc.what ());
        }

        QString what = QString ("Loading environment ") + ed.name_;
        foreach (ModelInterface* loader, pluginManager()->get <ModelInterface> ()) {
          QtConcurrent::run (loader, &ModelInterface::loadEnvironmentModel, ed);
          logString (what);
        }
      }
      statusBar()->clearMessage();
      e->close();
      e->deleteLater();
    }

    void MainWindow::handleWorkerDone(int /*id*/)
    {
    }

    void MainWindow::resetConnection()
    {
      foreach (ConnectionInterface* e, pluginManager()->get <ConnectionInterface> ()) {
        e->openConnection ();
      }
    }

    void MainWindow::closeConnection()
    {
      foreach (ConnectionInterface* e, pluginManager()->get <ConnectionInterface> ()) {
        e->closeConnection ();
      }
    }

#define _to_str_(a) #a
#define _to_str(a) _to_str_(a)
#define _osg_version_str _to_str(OPENSCENEGRAPH_MAJOR_VERSION) "." _to_str(OPENSCENEGRAPH_MINOR_VERSION) "." _to_str(OPENSCENEGRAPH_PATCH_VERSION)

    void MainWindow::about()
    {
      QString devString;
      devString = trUtf8("<p>Version %1. For more information visit <a href=\"%2\">%2</a></p>"
          "<p><ul>"
          "<li>Compiled with Qt %4, run with Qt %5</li>"
          "<li>Compiled with OpenSceneGraph version " _osg_version_str ", run with version %6</li>"
          "<li></li>"
          "<li></li>"
          "</ul></p>"
          "<p><small>Copyright (c) 2015-2016 CNRS<br/>By Joseph Mirabel and others.</small></p>"
          "<p><small>"
          "%3 is free software: you can redistribute it and/or modify it under the "
          "terms of the GNU Lesser General Public License as published by the Free "
          "Software Foundation, either version 3 of the License, or (at your option) "
          "any later version.<br/><br/>"
          "%3 is distributed in the hope that it will be "
          "useful, but WITHOUT ANY WARRANTY; without even the implied warranty "
          "of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU "
          "General Lesser Public License for more details.  You should have "
          "received a copy of the GNU Lesser General Public License along with %3. "
          "If not, see <a href=\"http://www.gnu.org/licenses\">http://www.gnu.org/licenses<a/>."
          "</small></p>"
          )
        .arg(QApplication::applicationVersion())
        .arg(QApplication::organizationDomain())
        .arg(QApplication::applicationName())
        .arg(QT_VERSION_STR)
        .arg(qVersion())
        .arg(osgGetVersion())
        ;

      QMessageBox::about(this, QApplication::applicationName(), devString);
    }

#undef _to_str
#undef _osg_version_str

    void MainWindow::activateCollision(bool activate)
    {
      if (activate) {
        requestConfigurationValidation();
        foreach (QString b, lastBodiesInCollision_) {
          osg ()->setHighlight(b.toLocal8Bit().data(), 1);
        }
      }
      else {
        foreach (QString b, lastBodiesInCollision_) {
          osg ()->setHighlight(b.toLocal8Bit().data(), 0);
        }
        collisionIndicator_->switchLed(true);
        collisionLabel_->setText("No collisions.");
      }
    }

    void MainWindow::dockVisibilityChanged(bool visible)
    {
      QWidget* cw = centralWidget();
      if (visible && cw->isVisible())
        cw->hide();
      else {
        const QObjectList& objs = children();
        foreach(const QObject* obj, objs) {
          const QDockWidget* dock = qobject_cast<const QDockWidget*>(obj);
          if (dock != 0 && dock->isVisible()) return;
        }
        cw->show();
      }
    }

    void MainWindow::hsplitTabifiedDockWidget()
    {
      splitTabifiedDockWidget(Qt::Horizontal);
    }

    void MainWindow::vsplitTabifiedDockWidget()
    {
      splitTabifiedDockWidget(Qt::Vertical);
    }

    QDockWidget* getParentDockWidget (QObject* child)
    {
      QDockWidget* dock = NULL;
      while (child != NULL) {
        dock = qobject_cast<QDockWidget*> (child);
        if (dock!=NULL) break;
        child = child->parent();
      }
      return dock;
    }

    void MainWindow::splitTabifiedDockWidget(Qt::Orientation orientation)
    {
      // QDockWidget focused
      QDockWidget* dock = getParentDockWidget (QApplication::focusWidget());
      if (dock==NULL) {
        qDebug() << "No QDockWidget focused";
        return;
      }
      // QDockWidget under cursor
      QDockWidget* other = getParentDockWidget (
          QApplication::widgetAt (QCursor::pos()));
      if (other==NULL) {
        qDebug() << "No QDockWidget under cursor";
        return;
      }
      if (other == dock) return;
      qDebug() << "Split " << dock->objectName() << other->objectName() << orientation;
      splitDockWidget(dock, other, orientation);
    }

    void MainWindow::setupInterface()
    {
      // Menu "Window/Tool bar"
      QMenu* toolbar = ui_->menuWindow->addMenu("Tool bar");
      toolbar->setIcon(QIcon::fromTheme("configure-toolbars"));
      ui_->mainToolBar->setVisible(true);
      toolbar->addAction (ui_->mainToolBar->toggleViewAction ());

      ui_->menuWindow->addSeparator();

      // Menu "Window/3D views"
      QMenu* views3D = ui_->menuWindow->addMenu("3D views");
      views3D->setObjectName("3d views");

      ui_->menuWindow->addSeparator();

      // Menu "Window"
      ui_->dockWidget_bodyTree->setVisible (false);
      ui_->dockWidget_bodyTree->toggleViewAction ()->setIcon(QIcon::fromTheme("window-new"));
      ui_->dockWidget_bodyTree->toggleViewAction ()->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_B);
      ui_->menuWindow->addAction(ui_->dockWidget_bodyTree->toggleViewAction ());
      ui_->dockWidget_log->setVisible (false);
      ui_->dockWidget_log->toggleViewAction ()->setIcon(QIcon::fromTheme("window-new"));
      ui_->dockWidget_log->toggleViewAction ()->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_L);
      ui_->menuWindow->addAction(ui_->dockWidget_log->toggleViewAction ());
#if GEPETTO_GUI_HAS_PYTHONQT
      insertDockWidget(pythonWidget_, Qt::BottomDockWidgetArea, Qt::Horizontal);
      registerShortcut("Python console", "Toggle view", pythonWidget_->toggleViewAction());
#endif

      // Add QActions to split dock widgets
      QAction* vsplit = new QAction("Split focused dock widget vertically", this);
      vsplit->setShortcut (Qt::CTRL + Qt::Key_S);
      addAction (vsplit);
      connect(vsplit, SIGNAL(triggered ()), SLOT(vsplitTabifiedDockWidget()));
      QAction* hsplit = new QAction("Split focused dock widget horizontally", this);
      hsplit->setShortcut (Qt::CTRL + Qt::Key_H);
      addAction (hsplit);
      connect(hsplit, SIGNAL(triggered ()), SLOT(hsplitTabifiedDockWidget()));

      registerShortcut("Log widget", "Toggle view", ui_->dockWidget_log->toggleViewAction ());
      registerShortcut("Body tree widget", "Toggle view", ui_->dockWidget_bodyTree->toggleViewAction ());
      registerShortcut("Main window", ui_->actionLoad_robot_from_file);
      registerShortcut("Main window", ui_->actionLoad_environment);
      registerShortcut("Main window", ui_->actionChange_shortcut);
      registerShortcut("Main window", ui_->actionQuit);
      registerShortcut("Main window", ui_->actionReconnect);
      registerShortcut("Main window", ui_->actionRefresh);
      registerShortcut("Main window", ui_->actionPlugins);
      registerShortcut("Main window", vsplit);
      registerShortcut("Main window", hsplit);

      ui_->menuWindow->addSeparator();

      // Setup the status bar
      collisionIndicator_ = new LedIndicator (statusBar());
      collisionValidationActivated_ = new QCheckBox ();
      collisionValidationActivated_->setToolTip (tr("Automatically validate configurations."));
      collisionValidationActivated_->setCheckState (Qt::Checked);
      statusBar()->addPermanentWidget(collisionLabel_);
      statusBar()->addPermanentWidget(collisionValidationActivated_);
      statusBar()->addPermanentWidget(collisionIndicator_);

      connect (collisionValidationActivated_, SIGNAL(clicked(bool)), SLOT(activateCollision(bool)));
      connect (collisionIndicator_, SIGNAL (mouseClickEvent()), SLOT(requestConfigurationValidation()));
      connect (ui_->actionAbout, SIGNAL (triggered ()), SLOT(about()));
      connect (ui_->actionReconnect, SIGNAL (triggered ()), SLOT(resetConnection()));
      connect (ui_->actionClose_connections, SIGNAL (triggered ()), SLOT(closeConnection()));

      connect (this, SIGNAL(logString(QString)), SLOT(log(QString)));
      connect (this, SIGNAL(logErrorString(QString)), SLOT(logError(QString)));

      // actionSearchBar_->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
      actionSearchBar_->setWindowFlags(Qt::Popup);
      actionSearchBar_->addAction(new NodeAction(NodeAction::VISIBILITY_ON,  "Show node", this));
      actionSearchBar_->addAction(new NodeAction(NodeAction::VISIBILITY_OFF, "Hide node", this));
      actionSearchBar_->addAction(new NodeAction(NodeAction::ALWAYS_ON_TOP, "Always on top", this));
      actionSearchBar_->addAction(ui_->actionFetch_configuration);
      actionSearchBar_->addAction(ui_->actionSend_configuration);
      actionSearchBar_->addAction(ui_->actionClose_connections);
      actionSearchBar_->addAction(ui_->actionReconnect);
      actionSearchBar_->addAction(ui_->actionRefresh);
    }

    void MainWindow::createCentralWidget()
    {
      if (!osgWindows_.empty()) return;
      createDefaultView();
    }

    void MainWindow::requestApplyCurrentConfiguration()
    {
      emit applyCurrentConfiguration();
      if (collisionValidationActivated_->isChecked ())
        requestConfigurationValidation();
    }

    void MainWindow::requestConfigurationValidation()
    {
      emit configurationValidation();
    }

    void MainWindow::onOpenPluginManager()
    {
      PluginManagerDialog d (pluginManager(), this);
      d.exec ();
    }

    void MainWindow::registerSignal(const char *signal, QObject *obj)
    {
      if (registeredSignals_.find(signal) == registeredSignals_.end())
        {
          registeredSignals_[signal] = obj;
          qDebug() << signal << " registered";
        }
      else
        qDebug() << "Signal" << signal << "already registered.";
    }

    QObject* MainWindow::getFromSignal(const char* signal)
    {
      if (registeredSignals_.find(signal) == registeredSignals_.end())
        {
          std::cout << "signal " << signal << "isn't registered" << std::endl;
          return NULL;
        }
      return registeredSignals_[signal];
    }

    void MainWindow::connectSignal(const char *signal, const char *slot, QObject* obj)
    {
      QObject* obj_sig = getFromSignal(signal);
      if (obj_sig != NULL) {
        QObject::connect(obj_sig, signal, obj, slot);
      }
    }

    void MainWindow::registerSlot(const char *slot, QObject *obj)
    {
      if (registeredSlots_.find(slot) == registeredSlots_.end())
        {
          registeredSlots_[slot] = obj;
          qDebug() << slot << " registered";
        }
      else
        std::cout << "Slot " << slot << "already registered." << std::endl;
    }

    QObject* MainWindow::getFromSlot(const char* slot)
    {
      if (registeredSlots_.find(slot) == registeredSlots_.end())
        {
          qDebug() << "slot" << slot << "isn't registered";
          return NULL;
        }
      return registeredSlots_[slot];
    }

    void MainWindow::connectSlot(const char *slot, const char *signal, QObject* obj)
    {
      if (registeredSlots_.find(slot) != registeredSlots_.end()) {
        QObject::connect(obj, signal, registeredSlots_[slot], slot);
      }
    }

    void MainWindow::registerShortcut(QString widgetName, QString actionName, QAction* action)
    {
      shortcutFactory_->addBinding(widgetName, actionName, action);
    }

    void MainWindow::registerShortcut(QString widgetName, QAction* action)
    {
      shortcutFactory_->addBinding(widgetName, action->text(), action);
    }

    void MainWindow::configurationValidationStatusChanged (bool valid)
    {
      collisionIndicator_->switchLed (valid);
      int state = (valid)?0:1;
      foreach(const QString& s, robotNames_) {
        osg ()->setHighlight(s.toLocal8Bit().data(), state);
      }
    }

    void MainWindow::configurationValidationStatusChanged (QStringList bodiesInCollision)
    {
      QStringList lastBodiesInCollision = lastBodiesInCollision_;
      lastBodiesInCollision_.clear();
      collisionIndicator_->switchLed (bodiesInCollision.empty());
      foreach (QString b, lastBodiesInCollision) {
        if (bodiesInCollision.removeAll(b) == 0) {
          /// This body is not in collision
          osg ()->setHighlight(b.toLocal8Bit().data(), 0);
        } else {
          /// This body is still in collision
          lastBodiesInCollision_.append(b);
        }
      }
      QString tooltip ("Collision between ");
      foreach(const QString& b, bodiesInCollision) {
        osg ()->setHighlight(b.toLocal8Bit().data(), 1);
        lastBodiesInCollision_.append(b);
      }
      tooltip += lastBodiesInCollision_.join (", ");
      if (lastBodiesInCollision_.count() > 0)
        collisionLabel_->setText(tooltip);
      else
        collisionLabel_->setText("No collisions.");
    }

    void MainWindow::requestSelectJointFromBodyName(const QString bodyName)
    {
      emit selectJointFromBodyName(bodyName);
    }
  } // namespace gui
} // namespace gepetto
