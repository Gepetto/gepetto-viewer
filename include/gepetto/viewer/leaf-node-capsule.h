//
//  leaf-node-capsule.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_LEAFNODECAPSULE_HH
#define GEPETTO_VIEWER_LEAFNODECAPSULE_HH

#include <gepetto/viewer/node-drawable.h>

namespace gepetto {
namespace viewer {
    DEF_CLASS_SMART_PTR(LeafNodeCapsule)
    
    /** Implementation of the Capsule GraphicalObject in OSG render
     * The capsule height is along Z.
     * */
    class LeafNodeCapsule : public NodeDrawable
    {
    private:
        /** Associated weak pointer */
        LeafNodeCapsuleWeakPtr weak_ptr_;
        /** Associated Capsule Shape */
        ::osg::CapsuleRefPtr capsule_ptr_;
        
        void init();
        
        /* Default constructor */
        LeafNodeCapsule(const std::string& name, const float& radius, const float& height);
        LeafNodeCapsule(const std::string& name, const float& radius, const float& height, const osgVector4& color);

        /* Copy constructor */
        LeafNodeCapsule(const LeafNodeCapsule& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr (LeafNodeCapsuleWeakPtr other_weak_ptr);
        
    protected:
    public:
        /** Static method which create a new box defined by the half_axis vector
         */
        static LeafNodeCapsulePtr_t create(const std::string& name, const float &radius, const float& height);
        static LeafNodeCapsulePtr_t create(const std::string& name, const float &radius, const float& height, const osgVector4& color);
        
        /** Static method for creating a clone of box other with the copy constructor
         */
        static LeafNodeCapsulePtr_t createCopy(const LeafNodeCapsulePtr_t& other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeCapsulePtr_t clone(void) const;
        
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual LeafNodeCapsulePtr_t copy() const { return clone(); }
        
        /** Return a shared pointer of the current object
         */
        LeafNodeCapsulePtr_t self(void) const;
        

        /** Fix the new radius of the capsule
         *  \note radius must be positive scalar
         */
        virtual void setRadius (const float& radius);

        float getRadius() const
        {
            return capsule_ptr_->getRadius();
        }


        
        /** Fix the new height of the capsule
         *  \note height must be positive scalar
         */
        virtual void setHeight (const float& height);
        float getHeight() const
        {
            return capsule_ptr_->getHeight();
        }

        SCENE_VIEWER_ACCEPT_VISITOR;

        /** Destructor */
        virtual ~LeafNodeCapsule();
    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LEAFNODECAPSULE_HH */
