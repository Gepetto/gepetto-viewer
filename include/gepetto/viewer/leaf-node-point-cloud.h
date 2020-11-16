//
//  leaf-node-point-cloud.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_LEAF_NODE_POINT_CLOUD_HH
#define GEPETTO_VIEWER_LEAF_NODE_POINT_CLOUD_HH

#include <gepetto/viewer/node.h>

namespace gepetto {
namespace viewer {
    DEF_CLASS_SMART_PTR(LeafNodePointCloud)
    
    class LeafNodePointCloud : public Node
    {
    private:
        /** Associated weak pointer */
        LeafNodePointCloudWeakPtr weak_ptr_;

        /** Associated Geometry for LeafNodePointCloud */
        ::osg::GeometryRefPtr geometry_ptr_;
        
        /** Associated colors */
        ::osg::Vec3ArrayRefPtr points_array_ptr_;
      
        /** Associated colors */
        ::osg::Vec4ArrayRefPtr colors_array_ptr_;
      
        /** Init method */
        void init();

        /** Constructor
         \brief Constructor from a point cloud and a list of colors
         
         \param name : name of the point cloud
         \param points : vector of points associated to the point cloud
         \param color : vector of colors to bind to each point
         */
        LeafNodePointCloud(const std::string & name,
                           const ::osg::Vec3ArrayRefPtr points,
                           const ::osg::Vec4ArrayRefPtr colors);
        
        /** Copy constructor */
        LeafNodePointCloud(const LeafNodePointCloud & other);
      
        /** Initialize weak_ptr */
        void initWeakPtr( const LeafNodePointCloudWeakPtr & other_weak_ptr);
        
    protected:
    public:
        
        /**
         \brief Build a PointCloud from a point cloud and a list of colors
         
         \param name : name of the point cloud
         \param points : vector of points associated to the point cloud
         \param colors : vector of colors to bind to each point
        */
        static LeafNodePointCloudPtr_t create(const std::string & name,
                                              const ::osg::Vec3ArrayRefPtr points,
                                              const ::osg::Vec4ArrayRefPtr colors);

      
        /**
         \brief Build a PointCloud from a point cloud and a single colot
         
         \param name : name of the point cloud
         \param points : vector of points associated to the point cloud
         \param color : a single color to bind to each point
         */
        static LeafNodePointCloudPtr_t create(const std::string & name,
                                              const ::osg::Vec3ArrayRefPtr points,
                                              const osgVector4 & color = osgVector4(1.,1.,1.,1.));
        
        /** Create copy */
        static LeafNodePointCloudPtr_t createCopy( const LeafNodePointCloudPtr_t & other);
        
        /** Clone
         \brief Returns a shared pointer to a newly allocated copy of the object.
         */
        virtual LeafNodePointCloudPtr_t clone(void) const;
      
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual LeafNodePointCloudPtr_t copy() const { return clone(); }
      
        /**
         \brief Returns a shared pointer of the object itself.
         */
        LeafNodePointCloudPtr_t self(void) const;

        /**
         * \brief Set a single to all the vertices
         */
        void setColor(const osgVector4 & color);
      
        /**
         \brief Change the color parameter associated to each node
         */
        virtual void setColors(const ::osg::Vec4ArrayRefPtr colors_array);
        
        /**
         \brief Change the color parameter associated to each node
         */
        virtual void setPoints(const ::osg::Vec3ArrayRefPtr points,
                               const ::osg::Vec4ArrayRefPtr colors_array);
        
        /**
         \brief Virtual method for setting the shadow mode of the object : receive, cast, both or no shadow
         \param shadow_state state
         */
        //virtual void setShadowMode (const ShadowMode& shadow_state) {
        //    Node::setShadowMode ( shadow_state );
        //}

        SCENE_VIEWER_ACCEPT_VISITOR;
        
        /** Destructor */
        virtual ~LeafNodePointCloud();
    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LEAF_NODE_POINT_CLOUD_HH */
