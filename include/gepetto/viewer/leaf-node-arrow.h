//
//  leaf-node-arrow.h
//  gepetto-viewer
//
//  Created by Pierre Fernbach in june 2015.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved
//

#ifndef SCENEVIEWER_LEAFNODEARROW_HH
#define SCENEVIEWER_LEAFNODEARROW_HH

#include <gepetto/viewer/node.h>
#include <osgDB/ReadFile>

namespace graphics {

    DEF_CLASS_SMART_PTR(LeafNodeArrow)

    /** Implementation of the XYZAxis GraphicalObject in OSG render */
    class LeafNodeArrow: public Node
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
        /** Associated Geometry (drawable objects)*/
        ::osg::GeodeRefPtr geode_ptr_;
        osgVector4 color_;

        void init ();


        /* Default constructor */
        LeafNodeArrow(const std::string& name, const osgVector4& color, float radiusCenter, float sizeAxis);


        /* Copy constructor */
        LeafNodeArrow(const LeafNodeArrow& other);

        /** Initialize weak_ptr */
        void initWeakPtr(LeafNodeArrowWeakPtr other_weak_ptr);




    protected:
    public:
        void addVertex (const osgVector3& vertex);
        /** Static method which create a new XYZ axis defined by the radius of the sphere (center) and size of the axis (0 by default)
         */
        static LeafNodeArrowPtr_t create(const std::string& name, const osgVector4& color, float radiusCenter, float sizeAxis);
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
        float getRadius() const;
        void setRadius (const float& radius);
        void setSize(const float& size);
        float getSize() const;
        void resize(float radius,float length);
        void setTexture(const std::string& image_path);

        /** Destructor */
        virtual ~LeafNodeArrow();

    };
} /* namespace graphics */

#endif /* SCENEVIEWER_LeafNodeArrow_HH */
