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
// gepetto-viewer not, see <http://www.gnu.org/licenses/>.

#ifndef GEPETTO_GUI_PLUGINMANAGERDIALOG_HH
#define GEPETTO_GUI_PLUGINMANAGERDIALOG_HH

#include <QDialog>
#include <QMap>
#include <QSignalMapper>
#include <QDir>
#include <QPluginLoader>
#include <QTableWidgetItem>

#include <gepetto/gui/plugin-interface.hh>

namespace Ui {
  class PluginManagerDialog;
}

namespace gepetto {
  namespace gui {
    class PluginManager {
      public:
        typedef QPair <QString, QPluginLoader*> Pair;
        typedef QMap <QString, QPluginLoader*> Map;
        typedef QPair <QString, QString> PyPair;
        typedef QMap <QString, QString> PyMap;

        ~PluginManager () {
          qDeleteAll (plugins_);
        }

        const Map& plugins () const { return plugins_; }

        const PyMap& pyplugins () const { return pyplugins_; }

        template <typename Interface> Interface* getFirstOf ();

        template <typename Interface> QList <Interface*> get ();

        static QIcon icon (const QPluginLoader* pl);

        static QString status (const QPluginLoader* pl);

        static void addPluginDir (const QString& path);

        void declareAllPlugins (QWidget* parent = NULL);

        bool declarePlugin (const QString& name, QWidget* parent = NULL);

        bool loadPlugin (const QString& name);

        bool initPlugin (const QString& name);

        bool unloadPlugin (const QString& name);

        void clearPlugins ();

        void declareAllPyPlugins ();

        bool declarePyPlugin (const QString& name);

        bool loadPyPlugin (const QString& name);

        bool unloadPyPlugin (const QString& name);

        bool isPyPluginLoaded (const QString& name);

        void clearPyPlugins ();

      private:
        template <typename Interface>
          static const Interface* const_instance_cast (const QPluginLoader* pl);

        QMap <QString, QPluginLoader*> plugins_;
        static QList <QDir> pluginDirs_;
        QMap <QString, QString> pyplugins_;
    };

    class PluginManagerDialog : public QDialog
    {
      Q_OBJECT

      public:
        explicit PluginManagerDialog(PluginManager* pm, QWidget *parent = 0);
        ~PluginManagerDialog();

      public slots:
        void onItemChanged (QTableWidgetItem* current, QTableWidgetItem* previous);
        void contextMenu(const QPoint& pos);
        void load (const QString& name);
        void unload (const QString& name);

        //void pyOnItemChanged (QTableWidgetItem* current, QTableWidgetItem* previous);
        void pyContextMenu(const QPoint& pos);
        void pyLoad (const QString& name);
        void pyUnload (const QString& name);

        void declareAll ();
        void save ();

      private:
        static const int P_NAME;
        static const int P_FILE;
        static const int P_FULLPATH;
        static const int P_VERSION;

        void updateList ();

        ::Ui::PluginManagerDialog *ui_;

        PluginManager* pm_;
    };

    template <typename Interface>
      Interface* PluginManager::getFirstOf ()
      {
        foreach (QPluginLoader* p, plugins_) {
          if (p->isLoaded()) {
            PluginInterface* check = qobject_cast <PluginInterface*> (p->instance());
            if (check && check->isInit ()) {
              Interface* pi = qobject_cast <Interface*> (p->instance());
              if (pi) return pi;
            }
          }
        }
        return NULL;
      }

    template <typename Interface>
      QList <Interface*> PluginManager::get ()
      {
        QList <Interface*> list;
        foreach (QPluginLoader* p, plugins_) {
          if (p->isLoaded()) {
            PluginInterface* check = qobject_cast <PluginInterface*> (p->instance());
            if (check && check->isInit ()) {
              Interface* pi = qobject_cast <Interface*> (p->instance());
              if (pi)
                list.append(pi);
            }
          }
        }
        return list;
      }

    template <typename Interface>
      const Interface* PluginManager::const_instance_cast (const QPluginLoader* pl)
      {
        return (const Interface*) qobject_cast <Interface*> (const_cast <QPluginLoader*>(pl)->instance());
      }
  } // namespace gui
} // namespace gepetto

#endif // GEPETTO_GUI_PLUGINMANAGERDIALOG_HH
