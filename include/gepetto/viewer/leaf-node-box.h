//
//  leaf-node-box.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_LEAFNODEBOX_HH
#define GEPETTO_VIEWER_LEAFNODEBOX_HH

#include <gepetto/viewer/node-drawable.h>

namespace gepetto {
namespace viewer {
    DEF_CLASS_SMART_PTR(LeafNodeBox)
    
    /** Implementation of the Box GraphicalObject in OSG render engine */
    class LeafNodeBox : public NodeDrawable
    {
    private:
        /** Associated weak pointer */
        LeafNodeBoxWeakPtr weak_ptr_;
        
        /** Associated Box Shape */
        ::osg::BoxRefPtr box_ptr_;
        
        void init ();
        
        /* Default constructor */
        LeafNodeBox (const std::string &name, const osgVector3 &half_axis);
        LeafNodeBox (const std::string &name, const osgVector3 &half_axis, const osgVector4 &color);
        
        /* Copy constructor */
        LeafNodeBox (const LeafNodeBox& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr (LeafNodeBoxWeakPtr other_weak_ptr);
        
    protected:
    public:
        /** Static method which create a new box defined by the half_axis vector
         */
        static LeafNodeBoxPtr_t create (const std::string &name, const osgVector3& half_axis);
        static LeafNodeBoxPtr_t create (const std::string &name, const osgVector3& half_axis, const osgVector4 &color);
        
        /** Static method for creating a clone of box other with the copy constructor
         */
        static LeafNodeBoxPtr_t createCopy (LeafNodeBoxPtr_t other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeBoxPtr_t clone (void) const;
        
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual NodePtr_t copy() const { return clone(); }
        
        /** Return a shared pointer of the current object 
         */
        LeafNodeBoxPtr_t self (void) const;
        
        /** Fix the new half axis of the box 
         * Note : half axis must be positive vector
         */
        void setHalfAxis (const osgVector3& half_axis);

        osgVector3 getHalfAxis () const
        {
            return box_ptr_->getHalfLengths();
        }

        SCENE_VIEWER_ACCEPT_VISITOR;

        /** Destructor */
        virtual ~LeafNodeBox();
        
    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LEAFNODEBOX_HH */
