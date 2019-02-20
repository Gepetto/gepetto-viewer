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

#ifndef GEPETTO_GUI_WINDOWSMANAGER_HH
#define GEPETTO_GUI_WINDOWSMANAGER_HH

#include <gepetto/gui/fwd.hh>
#include <gepetto/viewer/windows-manager.h>

#include <QColor>
#include <QObject>
#include <QVector3D>

namespace gepetto {
  namespace gui {
    class WindowsManager : public QObject, public viewer::WindowsManager
    {
      Q_OBJECT

      public:
        typedef viewer::WindowsManager Parent_t;

        static WindowsManagerPtr_t create (BodyTreeWidget* bodyTree);

        WindowID createWindow(const std::string& windowName);
        WindowID createWindow(const std::string& windowName,
                              OSGWidget* widget,
                              osgViewer::Viewer* viewer,
                              osg::GraphicsContext *gc);

        void insertNode (const std::string& nodeName, NodePtr_t node);
        bool addToGroup(const std::string& nodeName, const std::string& groupName);
        bool removeFromGroup (const std::string& nodeName, const std::string& groupName);
        bool deleteNode (const std::string& nodeName, bool all);

        BodyTreeItems_t bodyTreeItems (const std::string& name) const;

        void captureFrame (const WindowID windowId, const std::string& filename);
        bool startCapture (const WindowID windowId, const std::string& filename,
            const std::string& extension);
        bool stopCapture (const WindowID windowId);

        void refresh ();
        void setRefreshIsSynchronous (bool synchonous);

      public slots:
        int createWindow(QString windowName);
        void asyncRefresh ();

      protected:
        WindowsManager (BodyTreeWidget* bodyTree);

        virtual void addNode (const std::string& nodeName, NodePtr_t node, GroupNodePtr_t parent);
        virtual void addGroup(const std::string& groupName, GroupNodePtr_t group, GroupNodePtr_t parent);

      private:
        typedef std::pair<BodyTreeItems_t, bool> BodyTreeItemsAndGroup_t;
        typedef std::map<std::string, BodyTreeItemsAndGroup_t> BodyTreeItemMap_t;
        BodyTreeWidget* bodyTree_;
        BodyTreeItemMap_t nodeItemMap_;

        bool initParent (NodePtr_t node, GroupNodePtr_t parent, bool isGroup);
        void addToGroup (const std::string& nodeName, const std::string& groupName,
                         const NodePtr_t&   node,     const BodyTreeItems_t& groups,
                         bool isGroup);
        void deleteBodyItem(const std::string& nodeName);

        std::vector<OSGWidget*> widgets_;

        bool refreshIsSynchronous_;
        viewer::Mutex configsAsyncMtx_;
        NodeConfigurations_t configsAsync_;
    };
  } // namespace gui
} // namespace gepetto

#endif // GEPETTO_GUI_WINDOWSMANAGER_HH
