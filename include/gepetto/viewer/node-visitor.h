//
//  node-visitor.h
//  gepetto-viewer
//
//  Created by Joseph Mirabel in November 2015.
//  Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_NODE_VISITOR_HH
#define GEPETTO_VIEWER_NODE_VISITOR_HH

#include <iostream>
#include <gepetto/viewer/config-osg.h>

namespace gepetto {
namespace viewer {
    class Node;
    class GroupNode;
    class LeafNodeArrow;
    class LeafNodeBox;
    class LeafNodeCapsule;
    class LeafNodeCollada;
    class LeafNodeCone;
    class LeafNodeCylinder;
    class LeafNodeFace;
    class LeafNodeGround;
    class LeafNodeLight;
    class LeafNodeLine;
    class LeafNodeSphere;
    class LeafNodeXYZAxis;


    DEF_CLASS_SMART_PTR(NodeVisitor)

    class NodeVisitor : public osg::Referenced
    {
      public:
        NodeVisitor (bool invisibleAreValid = true) : invisibleAreValid_(invisibleAreValid) {}

        virtual ~NodeVisitor () {}

        virtual void apply (Node& node);
        virtual void apply (GroupNode& node);
        virtual void apply (LeafNodeArrow& node);
        virtual void apply (LeafNodeBox& node);
        virtual void apply (LeafNodeCapsule& node);
        virtual void apply (LeafNodeCollada& node);
        virtual void apply (LeafNodeCone& node);
        virtual void apply (LeafNodeCylinder& node);
        virtual void apply (LeafNodeFace& node);
        virtual void apply (LeafNodeGround& node);
        virtual void apply (LeafNodeLight& node);
        virtual void apply (LeafNodeLine& node);
        virtual void apply (LeafNodeSphere& node);
        virtual void apply (LeafNodeXYZAxis& node);

        virtual bool valid (Node& node);

        void traverse (Node& node);

      protected:
        bool invisibleAreValid_;
    }; /* class Node */
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_NODE_VISITOR_HH */
