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

#include "gepetto/gui/windows-manager.hh"

#include <gepetto/viewer/window-manager.h>
#include <gepetto/viewer/group-node.h>

#include "gepetto/gui/osgwidget.hh"
#include "gepetto/gui/mainwindow.hh"
#include "gepetto/gui/tree-item.hh"
#include <gepetto/gui/bodytreewidget.hh>

namespace gepetto {
  namespace gui {
    using viewer::ScopedLock;

    Qt::ConnectionType connectionType (QObject* o, int blocking)
    {
      if (o->thread() == QThread::currentThread())
        return Qt::DirectConnection;
      else if (blocking)
        return Qt::BlockingQueuedConnection;
      else
        return Qt::QueuedConnection;
    }

    WindowsManagerPtr_t WindowsManager::create(BodyTreeWidget* bodyTree)
    {
      return WindowsManagerPtr_t (new WindowsManager(bodyTree));
    }

    WindowsManager::WindowID WindowsManager::createWindow(QString windowName)
    {
      return createWindow(windowName.toStdString());
    }

    WindowsManager::WindowID WindowsManager::createWindow(const std::string& windowName)
    {
      if (getWindowManager(windowName, false))
        return windowName;
      MainWindow* main = MainWindow::instance();
      OSGWidget* widget;
      QMetaObject::invokeMethod (main, "createView",
          connectionType (main, true),
          Q_RETURN_ARG (OSGWidget*, widget),
          Q_ARG (std::string, windowName));
      return widget->windowID();
    }

    WindowsManager::WindowID WindowsManager::createWindow(const std::string& windowName,
                                                          gepetto::gui::OSGWidget* widget,
                                                          osgViewer::Viewer *viewer,
                                                          osg::GraphicsContext *gc)
    {
      if (getWindowManager(windowName, false))
        return windowName;
      viewer::WindowManagerPtr_t newWindow = viewer::WindowManager::create (viewer, gc);
      WindowID windowId = addWindow (windowName, newWindow);
      assert (windowId == windowName);
      widgets_.insert(std::make_pair(windowId, widget));
      return windowId;
    }

    WindowsManager::WindowsManager(BodyTreeWidget* bodyTree)
      : Parent_t ()
      , bodyTree_ (bodyTree)
      , refreshIsSynchronous_ (false)
    {
    }

    void WindowsManager::addNode(const std::string& nodeName, NodePtr_t node, GroupNodePtr_t parent)
    {
      Parent_t::addNode (nodeName, node, parent);
      if (parent)
        initParent(node, parent, false);
    }

    void WindowsManager::insertNode(const std::string& nodeName, NodePtr_t node)
    {
      ScopedLock lock(osgFrameMutex());
      addNode (nodeName, node, GroupNodePtr_t());
    }

    void WindowsManager::addGroup(const std::string& groupName, GroupNodePtr_t group, GroupNodePtr_t parent)
    {
      Parent_t::addGroup (groupName, group, parent);
      if (!parent || !initParent(group, parent, true)) {
        // Consider it a root group
        BodyTreeItem* bti = new BodyTreeItem (NULL, group);
        nodeItemMap_[groupName].first.push_back(bti);
        nodeItemMap_[groupName].second = true;
        if (bti->thread() != bodyTree_->thread())
          bti->moveToThread(bodyTree_->thread());
        bti->initialize();
        bodyTree_->model()->appendRow(bti);
      }
    }

    void WindowsManager::addToGroup (const std::string& nodeName, const std::string& groupName,
        const NodePtr_t& node, const BodyTreeItems_t& groups, bool isGroup)
    {
      for(std::size_t i = 0; i < groups.size(); ++i) {
        BodyTreeItem* bti = new BodyTreeItem (NULL, node);
        nodeItemMap_[nodeName].first.push_back(bti);
        nodeItemMap_[nodeName].second = isGroup;
        bti->setParentGroup(groupName);
        if (bti->thread() != bodyTree_->thread())
          bti->moveToThread(bodyTree_->thread());
        bti->initialize();
        groups[i]->appendRow(bti);
      }
    }

    bool WindowsManager::addToGroup(const std::string& nodeName, const std::string& groupName)
    {
      if (Parent_t::addToGroup(nodeName, groupName)) {
        NodePtr_t node = getNode(nodeName, false);
        bool isGroup = true;
        try {
            getGroup(nodeName, true);
        } catch (const std::invalid_argument& exc) {
            isGroup = false;
        }
        assert(node);
        BodyTreeItemMap_t::const_iterator _groups = nodeItemMap_.find(groupName);
        assert(_groups != nodeItemMap_.end());
        assert(!_groups->second.first.empty());
        assert(_groups->second.second);
        assert(getGroup(groupName));
        addToGroup(nodeName, groupName, node, _groups->second.first, isGroup);
        return true;
      }
      return false;
    }

    bool WindowsManager::removeFromGroup (const std::string& nodeName, const std::string& groupName)
    {
      bool ret = Parent_t::removeFromGroup(nodeName, groupName);
      if (ret) {
        BodyTreeItemMap_t::iterator _nodes  = nodeItemMap_.find(nodeName);
        BodyTreeItemMap_t::iterator _groups = nodeItemMap_.find(groupName);
        assert (_nodes  != nodeItemMap_.end());
        assert (_groups != nodeItemMap_.end());
        BodyTreeItems_t& nodes  = _nodes ->second.first;
        const BodyTreeItems_t& groups = _groups->second.first;
        bool found = false;
        for (BodyTreeItems_t::iterator _node = nodes.begin(); _node != nodes.end(); ++_node) {
          BodyTreeItems_t::const_iterator _group = std::find
            (groups.begin(), groups.end(), (*_node)->QStandardItem::parent());
          if (_group == groups.end()) continue;
          bodyTree_->model()->takeRow((*_node)->row());
          nodes.erase(_node);
          found = true;
          break;
        }
        if (!found)
          qDebug() << "Could not find body tree item parent" << groupName.c_str()
            << "of" << nodeName.c_str();
      }
      return ret;
    }

    bool WindowsManager::deleteNode (const std::string& nodeName, bool all)
    {
      bool ret = Parent_t::deleteNode(nodeName, all);
      if (ret) deleteBodyItem(nodeName);
      return ret;
    }

    BodyTreeItems_t WindowsManager::bodyTreeItems (const std::string& name) const
    {
      BodyTreeItemMap_t::const_iterator _btis = nodeItemMap_.find(name);
      if (_btis != nodeItemMap_.end())
        return _btis->second.first;
      return BodyTreeItems_t();
    }

    void WindowsManager::deleteBodyItem(const std::string& nodeName)
    {
      BodyTreeItemMap_t::iterator _nodes = nodeItemMap_.find(nodeName);
      assert (_nodes != nodeItemMap_.end());
      for (std::size_t i = 0; i < _nodes->second.first.size(); ++i) {
        BodyTreeItem* bti = _nodes->second.first[i];
        QStandardItem* parent = bti->QStandardItem::parent(); 
        if (parent == NULL) {
          bodyTree_->model()->takeRow(bti->row());
        } else {
          parent->takeRow(bti->row());
        }
        bti->deleteLater();
        _nodes->second.first[i] = NULL;
      }
      nodeItemMap_.erase(_nodes);
    }

    bool WindowsManager::initParent (NodePtr_t node,
        GroupNodePtr_t parent, bool isGroup)
    {
      BodyTreeItemMap_t::const_iterator _groups = nodeItemMap_.find(parent->getID());
      if (_groups != nodeItemMap_.end() && _groups->second.second) {
        assert(!_groups->second.first.empty());
        addToGroup(node->getID(), parent->getID(), node, _groups->second.first, isGroup);
        return true;
      }
      return false;
    }

    void WindowsManager::captureFrame (const WindowID wid, const std::string& filename)
    {
      WindowManagerPtr_t wm = getWindowManager(wid, true);
      OSGWidget* widget = widgets_[wid];
      assert(widget->windowID()==wid);
      // Here, it is not requred that invokeMethod is blocking. However, it may
      // be suprising in user script to have this call done later...
      QMetaObject::invokeMethod (widget, "captureFrame",
          connectionType (widget, true),
          Q_ARG (std::string, filename));
    }

    bool WindowsManager::startCapture (const WindowID wid, const std::string& filename,
            const std::string& extension)
    {
      WindowManagerPtr_t wm = getWindowManager(wid, true);
      OSGWidget* widget = widgets_[wid];
      assert(widget->windowID()==wid);
      bool res;
      QMetaObject::invokeMethod (widget, "startCapture",
          connectionType (widget, true),
          Q_RETURN_ARG (bool, res),
          Q_ARG (std::string, filename),
          Q_ARG (std::string, extension));
      return res;
    }

    bool WindowsManager::stopCapture (const WindowID wid)
    {
      WindowManagerPtr_t wm = getWindowManager(wid, true);
      OSGWidget* widget = widgets_[wid];
      assert(widget->windowID()==wid);
      bool res;
      QMetaObject::invokeMethod (widget, "stopCapture",
          connectionType (widget, true),
          Q_RETURN_ARG (bool, res));
      return res;
    }

    struct ApplyConfigurationFunctor
    {
      void operator() (const viewer::NodeConfiguration& nc) const
      {
        nc.node->applyConfiguration ( nc.position, nc.quat);
      }
    };

    void WindowsManager::refresh ()
    {
      if (refreshIsSynchronous_) {
        {
          ScopedLock lock(configListMtx_);
          {
            ScopedLock lock(osgFrameMutex());
            //refresh scene with the new configuration
            std::for_each(newNodeConfigurations_.begin(),
                newNodeConfigurations_.end(), ApplyConfigurationFunctor());
          }
          newNodeConfigurations_.resize (0);
        }
        if (autoCaptureTransform_) captureTransform ();
      } else {
        {
          ScopedLock lock1(configsAsyncMtx_);
          ScopedLock lock2(configListMtx_);
          if (configsAsync_.size() == 0) {
            configsAsync_.swap (newNodeConfigurations_);
            QMetaObject::invokeMethod (this, "asyncRefresh", Qt::QueuedConnection);
          } else {
            configsAsync_.insert (configsAsync_.end(),
                newNodeConfigurations_.begin(),
                newNodeConfigurations_.end());
            newNodeConfigurations_.resize(0);
            // No need to reinvoke asyncRefresh as it hasn't been ran yet.
          }
        }
      }
    }

    void WindowsManager::asyncRefresh ()
    {
      NodeConfigurations_t& cfgs = configsAsync_;
      {
        ScopedLock lock(configsAsyncMtx_);
        {
          ScopedLock lock(osgFrameMutex());
          //refresh scene with the new configuration
          std::for_each(cfgs.begin(), cfgs.end(), ApplyConfigurationFunctor());
        }
        cfgs.resize (0);
      }
      if (autoCaptureTransform_) captureTransform ();
    }

    void WindowsManager::setRefreshIsSynchronous (bool synchonous)
    {
      refreshIsSynchronous_ = synchonous;
    }
  } // namespace gui
} // namespace gepetto
