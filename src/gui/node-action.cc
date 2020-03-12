// Copyright (c) 2017, Joseph Mirabel
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer. If not, see <http://www.gnu.org/licenses/>.

#include <gepetto/gui/node-action.hh>

#include <gepetto/viewer/node.h>
#include <gepetto/viewer/window-manager.h>
#include <gepetto/gui/mainwindow.hh>
#include <gepetto/gui/selection-handler.hh>
#include <gepetto/gui/windows-manager.hh>
#include <gepetto/gui/osgwidget.hh>

namespace gepetto {
  namespace gui {
    NodeActionBase::NodeActionBase(const QString& text, NodePtr_t node, QWidget* parent)
      : QAction (text, parent)
      , node_ (node)
    {
      connect(this, SIGNAL(triggered(bool)), SLOT(_act(bool)));
    }

    NodePtr_t NodeActionBase::node () const
    {
      if (node_) return node_;
      QString s = MainWindow::instance()->selectionHandler()->mode()->currentBody();
      return MainWindow::instance()->osg()->getNode(s.toStdString(), false);
    }

    void NodeActionBase::_act (bool checked)
    {
      act(checked);
    }

    NodeAction::NodeAction(const NodeAction::Type& t, const QString& text, NodePtr_t node, QWidget* parent)
      : NodeActionBase (text, node, parent)
      , type_ (t)
    {}

    NodeAction::NodeAction(const NodeAction::Type& t, const QString& text, QWidget* parent)
      : NodeActionBase (text, NodePtr_t(), parent)
      , type_ (t)
    {}

    NodeAction::NodeAction(const QString& text, NodePtr_t node, OSGWidget* window, QWidget* parent)
      : NodeActionBase (text, node, parent)
      , type_ (ATTACH_TO_WINDOW)
      , window_ (window)
    {}

    NodeAction::NodeAction(const QString& text, OSGWidget* window, QWidget* parent)
      : NodeActionBase (text, NodePtr_t(), parent)
      , type_ (ATTACH_CAMERA_TO_NODE)
      , window_ (window)
    {}

    void NodeAction::act(bool)
    {
      NodePtr_t n = node();
      if (!n) return;
      switch (type_) {
        case VISIBILITY_ON:
          n->setVisibilityMode(viewer::VISIBILITY_ON);
          break;
        case VISIBILITY_OFF:
          n->setVisibilityMode(viewer::VISIBILITY_OFF);
          break;
        case ALWAYS_ON_TOP:
          n->setVisibilityMode(viewer::ALWAYS_ON_TOP);
          break;
        case ATTACH_TO_WINDOW:
          window_->osg()->addSceneToWindow(n->getID(), window_->windowID());
          break;
        case ATTACH_CAMERA_TO_NODE:
          window_->osg()->osgFrameMutex().lock();
          window_->window()->attachCameraToNode(n);
          window_->osg()->osgFrameMutex().unlock();
          break;
      }
    }
  }
}
