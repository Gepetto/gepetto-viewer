// Copyright (c) 2015, Joseph Mirabel
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
#include <gepetto/viewer/group-node.h>
#include <gepetto/viewer/leaf-node-arrow.h>
#include <gepetto/viewer/leaf-node-box.h>
#include <gepetto/viewer/leaf-node-capsule.h>
#include <gepetto/viewer/leaf-node-collada.h>
#include <gepetto/viewer/leaf-node-cone.h>
#include <gepetto/viewer/leaf-node-cylinder.h>
#include <gepetto/viewer/leaf-node-face.h>
#include <gepetto/viewer/leaf-node-ground.h>
#include <gepetto/viewer/leaf-node-light.h>
#include <gepetto/viewer/leaf-node-line.h>
#include <gepetto/viewer/leaf-node-sphere.h>
#include <gepetto/viewer/leaf-node-xyzaxis.h>
#include <gepetto/viewer/node-rod.h>
#include <gepetto/viewer/node-visitor.h>
#include <gepetto/viewer/roadmap-viewer.h>

namespace graphics {
  void NodeVisitor::apply (Node& node){
    traverse (node);
  }
  void NodeVisitor::apply (GroupNode& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeArrow& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeBox& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeCapsule& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeCollada& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeCone& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeCylinder& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeFace& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeGround& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeLight& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeLine& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeSphere& node){
    apply(static_cast<Node&>(node));
  }
  void NodeVisitor::apply (LeafNodeXYZAxis& node){
    apply(static_cast<Node&>(node));
  }
  bool NodeVisitor::valid (Node& node){
    if (invisibleAreValid_ || node.getVisibilityMode() != VISIBILITY_OFF)
      return true;
    return false;
  }
  void NodeVisitor::traverse (Node& node) {
    node.traverse (*this);
  }
} // namespace graphics
