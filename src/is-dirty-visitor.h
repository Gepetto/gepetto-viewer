// Copyright (c) 2019, Joseph Mirabel
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

#include <gepetto/viewer/node-visitor.h>
#include <gepetto/viewer/node.h>

namespace gepetto {
namespace viewer {
  class IsDirtyVisitor : public NodeVisitor
  {
    public:
      IsDirtyVisitor () : NodeVisitor (true), isDirty_ (false) {}

      ~IsDirtyVisitor () {}

      bool valid (Node& node)
      {
        if (isDirty_) return false;
        return NodeVisitor::valid(node);
      }

      void apply (Node& node)
      {
        isDirty_ = node.isDirty();
        if (isDirty_) return;
        // Invisible nodes must be considered, otherwise they do not
        // disappear when they become invisible. Their children can be discarded.
        if (node.getVisibilityMode() == VISIBILITY_OFF) return;
        NodeVisitor::apply (node);
      }

      bool isDirty () const
      {
        return isDirty_;
      }

    protected:
      bool isDirty_;
  }; /* class Node */
  template <bool SetDirty>
  class SetDirtyVisitorTpl : public NodeVisitor
  {
    public:
      SetDirtyVisitorTpl () : NodeVisitor (false) {}

      ~SetDirtyVisitorTpl () {}

      void apply (Node& node)
      {
        node.setDirty(SetDirty);
        NodeVisitor::apply (node);
      }
  }; /* class Node */
  typedef SetDirtyVisitorTpl<true > SetDirtyVisitor;
  typedef SetDirtyVisitorTpl<false> SetCleanVisitor;
} /* namespace viewer */
} // namespace gepetto
