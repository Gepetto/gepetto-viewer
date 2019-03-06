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

#ifndef GEPETTO_GUI_MAINWINDOW_HH
#define GEPETTO_GUI_MAINWINDOW_HH

#include "gepetto/gui/fwd.hh"

#include <QList>
#include <QStandardItemModel>
#include <QMainWindow>
#include <QCheckBox>
#include <QLabel>

#include <gepetto/viewer/group-node.h>

#include <gepetto/gui/ledindicator.hh>

#include <gepetto/gui/dialog/dialogloadrobot.hh>
#include <gepetto/gui/dialog/dialogloadenvironment.hh>
#include <gepetto/gui/dialog/pluginmanagerdialog.hh>

#include <gepetto/gui/settings.hh>

namespace Ui {
  class MainWindow;
}

namespace gepetto {
  namespace gui {
    /// Class
    class MainWindow : public QMainWindow
    {
      Q_OBJECT

      public:
        enum RefreshType {
          RefreshBodyTree = 1,
          RefreshPlugins  = 2,
          RefreshAll = RefreshBodyTree + RefreshPlugins
        };

        Settings* settings_;

        explicit MainWindow(Settings* settings, QWidget *parent = 0);
        ~MainWindow();

        /// Return the instance of MainWindow stored
        static MainWindow* instance ();

        /// Insert a dockwidget in the dock area of the window.
        /// \param dock widget to add
        /// \param area where the widget will be add
        /// \param orientation of the widget
        void insertDockWidget (QDockWidget* dock,
            Qt::DockWidgetArea area = Qt::AllDockWidgetAreas,
            Qt::Orientation orientation = Qt::Horizontal);

        /// Remove a dock widget.
        /// \param dock widget to remove
        void removeDockWidget (QDockWidget* dock);

        /// Get
        WindowsManagerPtr_t osg () const;

        SelectionHandler* selectionHandler()
        {
          return selectionHandler_;
        }

        /// Get the list of windows.
        QList <OSGWidget*> osgWindows () const;

        /// Get the plugin manager.
        PluginManager* pluginManager ();

        ActionSearchBar* actionSearchBar () const;

        QMenu* pluginMenu () const;

#if GEPETTO_GUI_HAS_PYTHONQT
        /// Get the python widget.
        PythonWidget* pythonWidget()
        {
          return pythonWidget_;
        }
#endif

signals:
        /// Triggered when an OSGWidget is created.
        void viewCreated (OSGWidget* widget);
        void refresh ();
        void applyCurrentConfiguration();
        void configurationValidation();
        void selectJointFromBodyName(const QString bodyName);

        void logString(QString msg);
        void logErrorString(QString msg);

        public slots:
          /// \ingroup plugin_python
          /// \{

        /// Get the body tree widget.
        BodyTreeWidget* bodyTree () const;

        /// Add the text to logs.
        /// \param text text to log
        void log (const QString& text);
        /// Add the text to logs and colors it in red.
        /// \param text text to log
        void logError (const QString& text);

        /// Log that a job has started.
        /// \param id id of the job
        /// \param text text to log
          void logJobStarted (int id, const QString& text);

          /// Log that a job has successfuly finished.
          /// \param id id of the job
          /// \param text text to log
          void logJobDone    (int id, const QString& text);

          /// Log that a job has failed.
          /// \param id id of the job
          /// \param text text to log
          void logJobFailed  (int id, const QString& text);

        /// Request a refresh of the interface.
        /// \param refreshType tells what to refresh. See RefreshType
        void requestRefresh ();
        /// Emit a signal to display the current configuration in the viewer.
        void requestApplyCurrentConfiguration ();

        /// Emit a signal to check if the the current configuration is valid.
        void requestConfigurationValidation ();

        /// Display if a configuration is valid or not.
        /// \param valid configuration is valid
        void configurationValidationStatusChanged (bool valid);

        /// Display if a configuration is valid or not.
        /// \param bodiesInCollision list of bodies in collision
        void configurationValidationStatusChanged (QStringList bodiesInCollision);

        /// Emit a signal to tell that a body has been selected.
        /// \param bodyName name of the body selected
        void requestSelectJointFromBodyName (const QString bodyName);

        /// Open the plugin manager dialog.
        void onOpenPluginManager ();

        /// Register an object signal that can be accessible without knowing the class definition.
        /// \param signal signal's name
        /// \param obj object's instance
        void registerSignal(const char *signal, QObject* obj);

        /// Get the instance of object which holds the signal.
        /// \param signal signal's name
        /// \return object's instance
        QObject* getFromSignal(const char *signal);

        /// Connect an object's slot to a registered signal.
        /// \param signal signal's name
        /// \param slot slot's name
        /// \param obj object's instance
        void connectSignal(const char *signal, const char *slot, QObject* obj);


        /// Register an object slot that can be accessible without knowing the class definition.
        /// \param slot slot's name
        /// \param obj object's instance
        void registerSlot(const char *slot, QObject* obj);

        /// Get the instance of object which holds the slot.
        /// \param slot slot's name
        /// \return object's instance
        QObject* getFromSlot(const char *slot);

        /// Connect an object's signal to a registered slot.
        /// \param slot slot's name
        /// \param signal signal's name
        /// \param obj object's instance
        void connectSlot(const char *slot, const char *signal, QObject* obj);

        /// Register an action on which users can change the shortcut to trigger it.
        /// \param widgetName widget's name that handle the action
        /// \param actionName action's name
        /// \param action action to modify
        void registerShortcut(QString widgetName, QString actionName, QAction* action);


        /// Register an action on which users can change the shortcut to trigger it.
        /// User action->text() as action name.
        /// \param widgetName widget's name that handle the action
        /// \param action action to modify
        void registerShortcut(QString widgetName, QAction* action);

        /// \}

        OSGWidget* createView (const std::string& name);


        private slots:
          void addOSGWidget(OSGWidget* osgWidget);
          void createDefaultView();
        void openLoadRobotDialog ();
        void openLoadEnvironmentDialog ();
        void activateCollision(bool activate);
        void dockVisibilityChanged(bool visible);
        void hsplitTabifiedDockWidget();
        void vsplitTabifiedDockWidget();

        void handleWorkerDone (int id);

        void resetConnection ();
        void closeConnection ();
        void about ();

      private:
        void splitTabifiedDockWidget(Qt::Orientation orientation);
        void setupInterface ();
        void createCentralWidget ();

        static MainWindow* instance_;

        ::Ui::MainWindow* ui_;
        OSGWidget* centralWidget_;
        QList <OSGWidget*> osgWindows_;
#if GEPETTO_GUI_HAS_PYTHONQT
        PythonWidget* pythonWidget_;
#endif
      ShortcutFactory* shortcutFactory_;
      SelectionHandler* selectionHandler_;

        WindowsManagerPtr_t osgViewerManagers_;
        QThread worker_;

        QCheckBox* collisionValidationActivated_;
        LedIndicator* collisionIndicator_;
        QLabel* collisionLabel_;

        QStandardItemModel *bodyTreeModel_;

        ActionSearchBar* actionSearchBar_;

        QStringList robotNames_;
        QStringList lastBodiesInCollision_;

        std::map<std::string, QObject *> registeredSlots_;
        std::map<std::string, QObject *> registeredSignals_;
    };
  } // namespace gui
} // namespace gepetto

Q_DECLARE_METATYPE (std::string)

#endif // GEPETTO_GUI_MAINWINDOW_HH
