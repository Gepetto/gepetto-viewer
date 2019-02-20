//
//  leaf-node-cylinder.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_LEAFNODECYLINDER_HH
#define GEPETTO_VIEWER_LEAFNODECYLINDER_HH

#include <gepetto/viewer/node-drawable.h>

namespace gepetto {
namespace viewer {
    DEF_CLASS_SMART_PTR(LeafNodeCylinder)
    
    /** Implementation of the Cylinder GraphicalObject in OSG render */
    class LeafNodeCylinder : public NodeDrawable
    {
    private:
        /** Associated weak pointer */
        LeafNodeCylinderWeakPtr weak_ptr_;
        
        /** Associated Cylinder Shape */
        ::osg::CylinderRefPtr cylinder_ptr_;
        
        void init();
        
        /* Default constructor */
        LeafNodeCylinder(const std::string &name, const float &radius, const float &height);
        LeafNodeCylinder(const std::string &name, const float &radius, const float &height,const  osgVector4 &color);
        
        /* Copy constructor */
        LeafNodeCylinder(const LeafNodeCylinder& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr (LeafNodeCylinderWeakPtr other_weak_ptr);
        
    protected:
    public:
        /** Static method which create a new box defined by the half_axis vector
         */
        static LeafNodeCylinderPtr_t create(const std::string &name, const float &radius, const float& height);
        static LeafNodeCylinderPtr_t create(const std::string &name, const float &radius, const float& height, const osgVector4 &color);

        /** Static method for creating a clone of box other with the copy constructor
         */
        static LeafNodeCylinderPtr_t createCopy(LeafNodeCylinderPtr_t other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeCylinderPtr_t clone(void) const;
        
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual NodePtr_t copy() const { return clone(); }
        
        /** Return a shared pointer of the current object
         */
        LeafNodeCylinderPtr_t self(void) const;

        /** Fix the new radius of the cylinder
         * Note : radius must be positive scalar
         */
        void setRadius (const float& radius);

        float getRadius () const
        {
            return cylinder_ptr_->getRadius();
        }

        
        /** Fix the new jeight of the cylinder
         * Note : radius must be positive scalar
         */
        void setHeight (const float& height);

        float getHeight () const
        {
            return cylinder_ptr_->getHeight();
        }

        SCENE_VIEWER_ACCEPT_VISITOR;
        
        /** Destructor */
        virtual ~LeafNodeCylinder();
        
    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LEAFNODECYLINDER_HH */
