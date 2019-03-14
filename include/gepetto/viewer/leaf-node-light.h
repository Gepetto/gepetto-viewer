//
//  leaf-node-light.h
//  gepetto-viewer
//
//  Created by Joseph Mirabel in July 2015.
//  Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_LEAFNODELIGHT_HH
#define GEPETTO_VIEWER_LEAFNODELIGHT_HH

#include <gepetto/viewer/leaf-node-sphere.h>
#include <gepetto/viewer/group-node.h>

namespace gepetto {
namespace viewer {
    DEF_CLASS_SMART_PTR(LeafNodeLight)

    /** Implementation of the Light GraphicalObject in OSG render */
    class LeafNodeLight : public LeafNodeSphere
    {
    private:
        /** Associated weak pointer */
        LeafNodeLightWeakPtr weak_ptr_;

        /** Associated Light Shape */
        ::osg::LightSourceRefPtr light_ptr_;

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
        LeafNodeLightPtr_t clone(void) const;

        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        LeafNodeLightPtr_t copy() const { return clone(); }

        /** Return a shared pointer of the current object
         */
        LeafNodeLightPtr_t self(void) const;

        /** Replace the old color pointer with the new one defined by ColorConstSharedPtr color_ptr */
        virtual void setColor (const osgVector4& color);

        void setRoot (GroupNodePtr_t root);

        SCENE_VIEWER_ACCEPT_VISITOR;

        /** Destructor */
        virtual ~LeafNodeLight();

    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LEAFNODELIGHT_HH */
