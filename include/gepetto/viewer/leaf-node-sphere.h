//
//  leaf-node-sphere.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_LEAFNODESPHERE_HH
#define GEPETTO_VIEWER_LEAFNODESPHERE_HH

#include <gepetto/viewer/node-drawable.h>

namespace gepetto {
namespace viewer {
    DEF_CLASS_SMART_PTR(LeafNodeSphere)
    
    /** Implementation of the Sphere GraphicalObject in OSG render */
    class LeafNodeSphere : public NodeDrawable
    {
    private:
        /** Associated weak pointer */
        LeafNodeSphereWeakPtr weak_ptr_;
        
        /** Associated Sphere Shape */
        ::osg::SphereRefPtr sphere_ptr_;
        
        void init();
        
    protected:
        /* Default constructor */
        LeafNodeSphere(const std::string& name, const float& radius);
        LeafNodeSphere(const std::string& name, const float& radius, const osgVector4& color);
        
        /* Copy constructor */
        LeafNodeSphere(const std::string& name, const LeafNodeSphere& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr (LeafNodeSphereWeakPtr other_weak_ptr);
        
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
        LeafNodeSpherePtr_t clone(void) const;
        
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        LeafNodeSpherePtr_t copy() const { return clone(); }
        
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
        
        SCENE_VIEWER_ACCEPT_VISITOR;
        
        /** Destructor */
        virtual ~LeafNodeSphere();
        
    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LEAFNODESPHERE_HH */
