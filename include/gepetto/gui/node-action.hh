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

#ifndef GEPETTO_GUI_NODE_ACTION_HH
#define GEPETTO_GUI_NODE_ACTION_HH

#include <gepetto/gui/fwd.hh>

#include <QAction>

#include <gepetto/viewer/node.h>

namespace gepetto {
  namespace gui {
    class NodeActionBase : public QAction {
      Q_OBJECT

    public:
        NodeActionBase(const QString& text, NodePtr_t node, QWidget* parent);

        virtual ~NodeActionBase () {}

        NodePtr_t node () const;

    protected:
      virtual void act(bool checked) = 0;

    private slots:
      void _act(bool checked);

    private:
      NodePtr_t node_;
    };

    class NodeAction : public NodeActionBase {
      public:
        enum Type {
          VISIBILITY_ON,
          VISIBILITY_OFF,
          ALWAYS_ON_TOP,
          ATTACH_TO_WINDOW,
          ATTACH_CAMERA_TO_NODE
        };

        NodeAction(const Type& t, const QString& text, NodePtr_t node, QWidget* parent);

        NodeAction(const Type& t, const QString& text, QWidget* parent);

        /// Attach to window
        NodeAction(const QString& text, NodePtr_t node, OSGWidget* window, QWidget* parent);

        /// Attach camera to node
        NodeAction(const QString& text, OSGWidget* window, QWidget* parent);

        virtual ~NodeAction () {}

      protected:
        void act(bool checked);

      private:
        const Type type_;
        OSGWidget* window_;
    };
  }
}

#endif // GEPETTO_GUI_NODE_ACTION_HH
