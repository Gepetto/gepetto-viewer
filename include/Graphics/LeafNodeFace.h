//
//  LeafNodeFace.h
//  SceneViewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_LEAFNODEFACE_HH
#define SCENEVIEWER_LEAFNODEFACE_HH

#include "Node.h"

namespace Graphics {
    DEF_CLASS_SMART_PTR(LeafNodeFace);
    
    /** Implementation of the Face GraphicalObject in OSG render */
    class LeafNodeFace : public Node
    {
    private:
        /** Associated weak pointer */
        LeafNodeFaceWeakPtr weak_ptr_;
        
        /** Associated Geometry for face*/
        ::osg::GeometryRefPtr face_ptr_;
        ::osg::GeodeRefPtr geode_face_ptr_;
        
        /** Associated Geometry for face*/
        ::osg::GeometryRefPtr edge_ptr_;
        ::osg::GeodeRefPtr geode_edge_ptr_;
        
        /** Associated SwitchNode to allow Face or Edge mode */
        ::osg::SwitchRefPtr switch_ref_ptr_;
        
        void init ();
        void createGeometry ();
        void updateGeometry ();
        
        void updateFaceGeometry ();
        void updateEdgeGeometry ();
        
        /* Default constructor */
        LeafNodeFace();
        
        /* Copy constructor */
        LeafNodeFace(const LeafNodeFace& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr(LeafNodeFaceWeakPtr other_weak_ptr);
        
    protected:
    public:
        /** Static method which create a new box defined by the half_axis vector
         */
        static LeafNodeFaceSharedPtr create();
        
        /** Static method for creating a clone of box other with the copy constructor
         */
        static LeafNodeFaceSharedPtr createCopy(LeafNodeFaceSharedPtr other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeFaceSharedPtr clone(void) const;
        
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual GraphicalObjectSharedPtr copy() const { return clone(); }
        
        /** Return a shared pointer of the current object
         */
        LeafNodeFaceSharedPtr self(void) const;
        
        /** Apply a new global configuration
         */
        virtual void applyConfiguration (::Tools::ConfigurationSharedPtr configuration_ptr)
        {
            Face::applyConfiguration( configuration_ptr);
            Node::applyConfiguration( configuration_ptr);
        }
        
        /** Define FaceMode */
        virtual void setFaceMode(const FaceMode& face_mode);
        
        /** Add a vertex to the vector of vertices
         */
        virtual bool addVertex (const DefVector3& vertex);
        
        /** Add a vertex to the vector of vertices at position i
         */
        virtual bool addVertex (unsigned int i, const DefVector3& vertex);
        
        /** Remove vertex at index i
         */
        virtual bool removeVertex (size_t i);
        
        /** Remove all vertices */
        virtual void removeAllVertices();
        
        /** Replace the old color pointer with the new one defined by ColorConstSharedPtr color_ptr */
        virtual void setFaceColor (ColorConstSharedPtr color_ptr);
        
        /** Replace the old color with the new one defined by DefVector3 color_vector */
        virtual void setFaceColor (const DefVector3& color_vector) { LeafNodeFace::setFaceColor(::Graphics::Color::create(color_vector, getAlpha())); }
        
        /** Define the new color of the edges
         */
        virtual void setEdgeColor (ColorConstSharedPtr edge_color_ptr);
        
        /** Define the new color of the edges
         */
        virtual void setEdgeColor (const DefVector3& color_vector) { LeafNodeFace::setEdgeColor(::Graphics::Color::create(color_vector, getAlpha())); }
        
        /** Replace the old transparency value with alpha
         *  Note : alpha must be in [0,1]
         */
        virtual void setAlpha (const DefScalar& alpha);
        
        /** Define the new scale of the cone */
        virtual void setScale (const DefScalar& scale);
        
        /**
         \brief Virtual method for setting the visibility mode of the object : visible, not visible or always on top
         \param visibilitymode state
         */
        virtual void setVisibilityMode (const VisibilityMode& visibility_state)
        {
            GraphicalObject::setVisibilityMode ( visibility_state );
            Node::setVisibilityMode ( visibility_state );
        }
        
        /**
         \brief Virtual method for setting the lighting mode of the object : influence by light or not
         \param lightingmode state
         */
        virtual void setLightingMode (const LightingMode& lighting_state)
        {
            GraphicalObject::setLightingMode ( lighting_state );
            Node::setLightingMode ( lighting_state );
        }
        
        /**
         \brief Virtual method for setting the wireframe mode of the object : visible or not
         \param wireframemode state
         */
        virtual void setWireFrameMode (const WireFrameMode& wireframe_state)
        {
            GraphicalObject::setWireFrameMode ( wireframe_state );
            Node::setWireFrameMode ( wireframe_state );
        }
        
        /** Destructor */
        virtual ~LeafNodeFace();
        
    };
} /* namespace Graphics */

#endif /* SCENEVIEWER_LEAFNODEFACE_HH */
