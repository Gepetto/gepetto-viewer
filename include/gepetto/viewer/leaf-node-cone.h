//
//  leaf-node-cone.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_LEAFNODECONE_HH
#define GEPETTO_VIEWER_LEAFNODECONE_HH

#include <gepetto/viewer/node-drawable.h>

namespace gepetto {
namespace viewer {
    DEF_CLASS_SMART_PTR(LeafNodeCone)
    
    /** Implementation of the Cone GraphicalObject in OSG render */
    class LeafNodeCone : public NodeDrawable
    {
    private:
        /** Associated weak pointer */
        LeafNodeConeWeakPtr weak_ptr_;
        
        /** Associated Cone Shape */
        ::osg::ConeRefPtr cone_ptr_;
        
        void init();
        
        /* Default constructor */
        LeafNodeCone(const std::string& name, const float& radius, const float& height);
        LeafNodeCone(const std::string& name, const float& radius, const float& height, const osgVector4& color);
        
        /* Copy constructor */
        LeafNodeCone(const LeafNodeCone& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr (LeafNodeConeWeakPtr other_weak_ptr);
        
    protected:
    public:
        /** Static method which create a new box defined by the half_axis vector
         */
        static LeafNodeConePtr_t create(const std::string& name, const float &radius, const float& height);
        static LeafNodeConePtr_t create(const std::string& name, const float &radius, const float& height, const osgVector4& color);
        
        /** Static method for creating a clone of box other with the copy constructor
         */
        static LeafNodeConePtr_t createCopy(LeafNodeConePtr_t other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeConePtr_t clone(void) const;
        
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual LeafNodeConePtr_t copy() const { return clone(); }
        
        /** Return a shared pointer of the current object
         */
        LeafNodeConePtr_t self(void) const;
        
        /** Fix the new radius of the cone
         * Note : radius must be positive scalar
         */
        virtual void setRadius (const float& radius);

        float getRadius() const
        {
            return cone_ptr_->getRadius();
        }

        /** Fix the new jeight of the cone
         * Note : radius must be positive scalar
         */
        virtual void setHeight (const float& height);

        float getHeight() const
        {
            return cone_ptr_->getHeight();
        }

        SCENE_VIEWER_ACCEPT_VISITOR;
        
        /** Destructor */
        virtual ~LeafNodeCone();
        
    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LEAFNODECONE_HH */
