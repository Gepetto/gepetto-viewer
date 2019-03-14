//
//  transform-writer.h
//  gepetto-viewer
//
//  Created by Joseph Mirabel in November 2015.
//  Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_BLENDER_GEOM_WRITER_VISITOR_HH
#define GEPETTO_VIEWER_BLENDER_GEOM_WRITER_VISITOR_HH

#include <iostream>
#include <fstream>
#include <stack>
#include <gepetto/viewer/config-osg.h>
#include <gepetto/viewer/node-visitor.h>

namespace gepetto {
namespace viewer {

    DEF_CLASS_SMART_PTR(BlenderGeomWriterVisitor)

    class BlenderGeomWriterVisitor : public NodeVisitor
    {
      public:
        BlenderGeomWriterVisitor (const std::string& filename);

        // virtual ~BlenderGeomWriter () {}

        void apply (Node& node);
        void apply (GroupNode& node);
        void apply (LeafNodeArrow& node);
        void apply (LeafNodeBox& node);
        void apply (LeafNodeCapsule& node);
        void apply (LeafNodeCollada& node);
        void apply (LeafNodeCone& node);
        void apply (LeafNodeCylinder& node);
        void apply (LeafNodeFace& node);
        void apply (LeafNodeGround& node);
        void apply (LeafNodeLight& node);
        void apply (LeafNodeLine& node);
        void apply (LeafNodeSphere& node);
        void apply (LeafNodeXYZAxis& node);

      protected:
        std::ofstream& out () { return file_; }

        // The scale multiplier is requited for LeafNodeBox
        void standardApply (Node& node, osgVector3 scaleMultiplier = osgVector3(1,1,1));

        bool openFile ();
        void closeFile () {
          file_.close ();
        }
        void unimplemented(const char* type, Node& n);

        std::ofstream file_;
        std::string filename_;
        unsigned int nodeCount_;
        std::size_t groupDepth_;
    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_BLENDER_GEOM_WRITER_VISITOR_HH */
