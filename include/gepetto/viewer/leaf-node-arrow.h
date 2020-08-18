//
//  leaf-node-arrow.h
//  gepetto-viewer
//
//  Created by Pierre Fernbach in june 2015.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved
//

#ifndef GEPETTO_VIEWER_LEAFNODEARROW_HH
#define GEPETTO_VIEWER_LEAFNODEARROW_HH

#include <gepetto/viewer/node-drawable.h>

namespace gepetto {
namespace viewer {

    DEF_CLASS_SMART_PTR(LeafNodeArrow)

    /** Implementation of the XYZAxis GraphicalObject in OSG render */
    class LeafNodeArrow: public NodeDrawable
    {
    private:
        /** Associated weak pointer */
        LeafNodeArrowWeakPtr weak_ptr_;

        /** Sphere component */
        float radius_;
        /** axis components : */
        float size_;
        ::osg::ShapeDrawableRefPtr cylinder_drawable_;
        ::osg::ShapeDrawableRefPtr cone_drawable_;

        osgVector4 color_;

        void init ();

        void resetGeodeContent ();


        /* Default constructor */
        LeafNodeArrow(const std::string& name, const osgVector4& color, float radiusCenter, float sizeAxis);


        /* Copy constructor */
        LeafNodeArrow(const LeafNodeArrow& other);

        /** Initialize weak_ptr */
        void initWeakPtr(LeafNodeArrowWeakPtr other_weak_ptr);




    protected:
    public:
        void addVertex (const osgVector3& vertex);
        /** Static method which create a new arrow defined by the radius and length of the arrow.
         * See \ref getSize and \ref getRadius
         */
        static LeafNodeArrowPtr_t create(const std::string& name, const osgVector4& color, float radiusCenter, float sizeAxis);
        /** Static method which create a new arrow without cylindrical base.
         */
        static LeafNodeArrowPtr_t create (const std::string& name, const osgVector4& color, float radiusCenter);

        /** Static method for creating a clone of box other with the copy constructor
         */
        static LeafNodeArrowPtr_t createCopy(LeafNodeArrowPtr_t other);

        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeArrowPtr_t clone(void) const;

        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual NodePtr_t copy() const { return clone(); }

        /** Return a shared pointer of the current object
         */
        LeafNodeArrowPtr_t self(void) const;

        // set color
        void setColor (const osgVector4& color);
        osgVector4 getColor () const
        {
          return color_;
        }
        /// Get the radius of the arrow.
        /// It is the cylinder radius. The cone base radius is twice larger.
        float getRadius() const;
        void setRadius (const float& radius);
        void setSize(const float& size);
        /// Get the length of the arrow.
        /// It is the sum of the cylinder and cone length.
        float getSize() const;
        void resize(float radius,float length);

        SCENE_VIEWER_ACCEPT_VISITOR;

        /** Destructor */
        virtual ~LeafNodeArrow();

    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LeafNodeArrow_HH */
