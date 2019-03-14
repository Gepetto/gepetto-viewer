//
//  roadmap-viewer.h
//  gepetto-viewer
//
//  Created by Pierre Fernbach in april 2015.
//  Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_ROADMAPVIEWER_HH
#define GEPETTO_VIEWER_ROADMAPVIEWER_HH

#include <OpenThreads/Mutex>

#include <gepetto/viewer/node.h>
#include <gepetto/viewer/group-node.h>
#include <gepetto/viewer/leaf-node-xyzaxis.h>
#include <gepetto/viewer/leaf-node-line.h>

namespace gepetto {
namespace viewer {

    DEF_CLASS_SMART_PTR(RoadmapViewer)

    class RoadmapViewer : public Node
    {
    private:
        /**
         \brief List of all child graphical object
         */
        std::list<LeafNodeXYZAxisPtr_t> list_nodes_;
        std::list<LeafNodeLinePtr_t> list_edges_;

        /** Associated weak pointer */
        RoadmapViewerWeakPtr weak_ptr_;
        // ---
        float radiusSphere_;
        float sizeAxis_;
        osgVector4 colorNode_;
        osgVector4 colorEdge_;
        /** Initialize weak_ptr */
        void initWeakPtr(RoadmapViewerWeakPtr other_weak_ptr);

    protected:
        /**
         \brief Default constructor
         */
        RoadmapViewer (const std::string& name,const osgVector4& colorNode,float radiusSphere,float sizeAxis,const osgVector4& colorEdge);

        /** Copy constructor */
        RoadmapViewer (const RoadmapViewer& other);

    public:
        /** Static method which create a new box defined by the half_axis vector
         */
       static RoadmapViewerPtr_t create(const std::string& name,const osgVector4& colorNode,float radiusSphere,float sizeAxis,const osgVector4& colorEdge);

        /** Static method for creating a clone of box other with the copy constructor
         */
        static RoadmapViewerPtr_t createCopy(RoadmapViewerPtr_t other);

        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual RoadmapViewerPtr_t clone(void) const;

        /** Return a shared pointer of the current object
         */
        RoadmapViewerPtr_t self(void) const;

        bool addNode(osgVector3 position, osgQuat quat, ::OpenThreads::Mutex& mtx);

        bool addEdge(osgVector3 from, osgVector3 to, ::OpenThreads::Mutex& mtx);

        virtual void removeAllChildren();

        /**
         \brief Virtual method for setting the visibility mode of the object : visible or not visible
         \param visibilitymode state
         */
        virtual void setVisibilityMode (const VisibilityMode& visibility_state);

        /**
         \brief Virtual method for setting the lighting mode of the object : influence by light or not
         \param lightingmode state
         */
        virtual void setLightingMode (const LightingMode& lighting_state);

        /**
         \brief Virtual method for setting the wireframe mode of the object : visible or not
         \param wireframemode state
         */
        virtual void setWireFrameMode (const WireFrameMode& wireframe_state);

        virtual size_t getNumOfNodes() const
        {
          return list_nodes_.size();
        }

        virtual size_t getNumOfEdges() const
        {
          return list_edges_.size();
        }

        virtual LeafNodeXYZAxisPtr_t getNode(size_t i) const
        {
           std::list<LeafNodeXYZAxisPtr_t>::const_iterator it = list_nodes_.begin();
           if (list_nodes_.size() > i)
           {
               std::advance(it, i);
           }
           return *it;
        }

        virtual LeafNodeLinePtr_t getEdge(size_t i) const
        {
           std::list<LeafNodeLinePtr_t>::const_iterator it = list_edges_.begin();
           if (list_edges_.size() > i)
           {
               std::advance(it, i);
           }
           return *it;
        }

        virtual float getRadiusSphere() const {
            return radiusSphere_;
        }

        virtual float getSizeAxis() const {
            return sizeAxis_;
        }

        virtual osgVector4 getColorNode() const{
            return colorNode_;
        }

        virtual osgVector4 getColorEdge() const{
            return colorEdge_;
        }

        void setColorNode(const osgVector4& color);

        void setColorEdge(const osgVector4& color);

        void setColor (const osgVector4& color) {
            setColorEdge (color);
        }

    }; //class

} /* namespace viewer */
} /* namespace gepetto */

#endif
