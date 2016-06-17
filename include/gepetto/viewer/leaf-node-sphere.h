//
//  leaf-node-sphere.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_LEAFNODESPHERE_HH
#define SCENEVIEWER_LEAFNODESPHERE_HH

#include <gepetto/viewer/node.h>
#include <osgDB/ReadFile>

namespace graphics {
    DEF_CLASS_SMART_PTR(LeafNodeSphere)
    
    /** Implementation of the Sphere GraphicalObject in OSG render */
    class LeafNodeSphere : public Node
    {
    private:
        /** Associated weak pointer */
        LeafNodeSphereWeakPtr weak_ptr_;
        
        /** Associated Sphere Shape */
        ::osg::SphereRefPtr sphere_ptr_;
        
        /** Associated ShapeDrawable */
        ::osg::ShapeDrawableRefPtr shape_drawable_ptr_;
        
        void init();
        
        /* Default constructor */
        LeafNodeSphere(const std::string& name, const float& radius);
        LeafNodeSphere(const std::string& name, const float& radius, const osgVector4& color);
        
        /* Copy constructor */
        LeafNodeSphere(const std::string& name, const LeafNodeSphere& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr (LeafNodeSphereWeakPtr other_weak_ptr);
        
    protected:
    public:
        /** Static method which create a new box defined by the half_axis vector
         */
        static LeafNodeSpherePtr_t create(const std::string& name, const float &radius);
        static LeafNodeSpherePtr_t create(const std::string& name, const float &radius, const osgVector4& color);
        
        /** Static method for creating a clone of sphere other with the copy constructor
         */
        static LeafNodeSpherePtr_t createCopy(LeafNodeSpherePtr_t other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeSpherePtr_t clone(void) const;
        
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual LeafNodeSpherePtr_t copy() const { return clone(); }
        
        /** Return a shared pointer of the current object
         */
        LeafNodeSpherePtr_t self(void) const;

        
        /** Fix the new radius of the sphere
         * Note : radius must be positive vector
         */
        void setRadius (const float& radius);
        float getRadius() const
        {
            return sphere_ptr_->getRadius();
        }
        
        /** Replace the old color pointer with the new one defined by ColorConstSharedPtr color_ptr */
        virtual void setColor (const osgVector4& color);

        osgVector4 getColor() const
        {
            return shape_drawable_ptr_->getColor();
        }
        
        void setTexture(const std::string& image_path);

        SCENE_VIEWER_ACCEPT_VISITOR;
        
        /** Destructor */
        virtual ~LeafNodeSphere();
        
    };
} /* namespace graphics */

#endif /* SCENEVIEWER_LEAFNODESPHERE_HH */
