//
//  leaf-node-xyzaxis.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert, Pierre Fernbach in avril 2015.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved
//

#ifndef SCENEVIEWER_LEAFNODEXYZ_HH
#define SCENEVIEWER_LEAFNODEXYZ_HH

#include <gepetto/viewer/node.h>
#include <osgDB/ReadFile>

namespace graphics {

    DEF_CLASS_SMART_PTR(LeafNodeXYZAxis)

    /** Implementation of the XYZAxis GraphicalObject in OSG render */
    class LeafNodeXYZAxis: public Node
    {
    private:
        /** Associated weak pointer */
        LeafNodeXYZAxisWeakPtr weak_ptr_;

        /** Sphere component */
        float radius_;
        ::osg::SphereRefPtr sphere_ptr_;
        ::osg::ShapeDrawableRefPtr sphere_drawable_ptr_;

        /** axis components : */
        float sizeAxis_;
        /** Associated Geometry (drawable objects)*/
        ::osg::Vec4ArrayRefPtr color_ptr_;

        void init ();


        /* Default constructor */
        LeafNodeXYZAxis(const std::string& name, const osgVector4& color, float radiusCenter, float sizeAxis);


        /* Copy constructor */
        LeafNodeXYZAxis(const LeafNodeXYZAxis& other);

        /** Initialize weak_ptr */
        void initWeakPtr(LeafNodeXYZAxisWeakPtr other_weak_ptr);

    protected:
    public:
        void addVertex (const osgVector3& vertex);
        /** Static method which create a new XYZ axis defined by the radius of the sphere (center) and size of the axis (0 by default)
         */
        static LeafNodeXYZAxisPtr_t create(const std::string& name, const osgVector4& color, float radiusCenter, float sizeAxis);
        static LeafNodeXYZAxisPtr_t create (const std::string& name, const osgVector4& color, float radiusCenter);

        /** Static method for creating a clone of box other with the copy constructor
         */
        static LeafNodeXYZAxisPtr_t createCopy(LeafNodeXYZAxisPtr_t other);

        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeXYZAxisPtr_t clone(void) const;

        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual NodePtr_t copy() const { return clone(); }

        /** Return a shared pointer of the current object
         */
        LeafNodeXYZAxisPtr_t self(void) const;

        // set color
        void setColor (const osgVector4& color);
        float getRadius() const;
        void setRadius (const float& radius);
        void setSizeAxis(const float& sizeAxis);
        float getSizeAxis() const;
        void setTexture(const std::string& image_path);

        SCENE_VIEWER_ACCEPT_VISITOR;

        /** Destructor */
        virtual ~LeafNodeXYZAxis();

    };
} /* namespace graphics */

#endif /* SCENEVIEWER_LEAFNODEXYZAXIS_HH */
