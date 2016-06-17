//
//  leaf-node-box.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_LEAFNODEBOX_HH
#define SCENEVIEWER_LEAFNODEBOX_HH

#include <gepetto/viewer/node.h>
#include <osgDB/ReadFile>

namespace graphics {
    DEF_CLASS_SMART_PTR(LeafNodeBox)
    
    /** Implementation of the Box GraphicalObject in OSG render engine */
    class LeafNodeBox : public Node
    {
    private:
        /** Associated weak pointer */
        LeafNodeBoxWeakPtr weak_ptr_;
        
        /** Associated Box Shape */
        ::osg::BoxRefPtr box_ptr_;
        
        /** Associated ShapeDrawable */
        ::osg::ShapeDrawableRefPtr shape_drawable_ptr_;
        
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

        void setColor(const osgVector4 &color);

        osgVector4 getColor() const
        {
            return shape_drawable_ptr_->getColor();
        }
        
        void setTexture(const std::string& image_path);

        SCENE_VIEWER_ACCEPT_VISITOR;

        /** Destructor */
        virtual ~LeafNodeBox();
        
    };
} /* namespace graphics */

#endif /* SCENEVIEWER_LEAFNODEBOX_HH */
