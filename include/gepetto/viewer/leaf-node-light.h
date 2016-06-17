//
//  leaf-node-light.h
//  gepetto-viewer
//
//  Created by Joseph Mirabel in July 2015.
//  Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_LEAFNODELIGHT_HH
#define SCENEVIEWER_LEAFNODELIGHT_HH

#include <gepetto/viewer/node.h>
#include <gepetto/viewer/group-node.h>

#include <osgDB/ReadFile>

namespace graphics {
    DEF_CLASS_SMART_PTR(LeafNodeLight)

    /** Implementation of the Light GraphicalObject in OSG render */
    class LeafNodeLight : public Node
    {
    private:
        /** Associated weak pointer */
        LeafNodeLightWeakPtr weak_ptr_;

        /** Associated Light Shape */
        ::osg::LightSourceRefPtr light_ptr_;
        ::osg::SphereRefPtr bulb_ptr_;

        /** Associated ShapeDrawable */
        ::osg::ShapeDrawableRefPtr shape_drawable_ptr_;

        void init();

        /* Default constructor */
        LeafNodeLight(const std::string& name, const float& radius);
        LeafNodeLight(const std::string& name, const float& radius, const osgVector4& color);

        /* Copy constructor */
        LeafNodeLight(const std::string& name, const LeafNodeLight& other);

        /** Initialize weak_ptr */
        void initWeakPtr (LeafNodeLightWeakPtr other_weak_ptr);

        static int uniqueLightNumber;

    protected:
    public:
        /** Static method which create a new box defined by the half_axis vector
         */
        static LeafNodeLightPtr_t create(const std::string& name, const float &radius);
        static LeafNodeLightPtr_t create(const std::string& name, const float &radius, const osgVector4& color);

        /** Static method for creating a clone of light other with the copy constructor
         */
        static LeafNodeLightPtr_t createCopy(LeafNodeLightPtr_t other);

        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeLightPtr_t clone(void) const;

        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual LeafNodeLightPtr_t copy() const { return clone(); }

        /** Return a shared pointer of the current object
         */
        LeafNodeLightPtr_t self(void) const;


        /** Fix the new radius of the light
         * Note : radius must be positive vector
         */
        void setRadius (const float& radius);
        float getRadius() const
        {
            return bulb_ptr_->getRadius();
        }

        /** Replace the old color pointer with the new one defined by ColorConstSharedPtr color_ptr */
        virtual void setColor (const osgVector4& color);

        osgVector4 getColor() const
        {
            return shape_drawable_ptr_->getColor();
        }

        void setTexture(const std::string& image_path);

        void setRoot (GroupNodePtr_t root);

        SCENE_VIEWER_ACCEPT_VISITOR;

        /** Destructor */
        virtual ~LeafNodeLight();

    };
} /* namespace graphics */

#endif /* SCENEVIEWER_LEAFNODELIGHT_HH */
