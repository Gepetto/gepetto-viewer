//
//  leaf-node-mesh.h
//  gepetto-viewer
//
//  Created by Olivier Stasse, Justin Carpentier, Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2016 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_LEAFNODEMESH_HH
#define GEPETTO_VIEWER_LEAFNODEMESH_HH

#include <gepetto/viewer/node.h>
#include <osg/PrimitiveSet>
#include <osg/Geometry>

namespace gepetto {
namespace viewer {
    DEF_CLASS_SMART_PTR(LeafNodeMesh)
    
    /** Implementation of the Mesh GraphicalObject in OSG render */
    class LeafNodeMesh : public Node
    {
    private:

        /** Associated weak pointer */
        LeafNodeMeshWeakPtr weak_ptr_;
        
        /** Associated mesh geometry */
	osg::GeometryRefPtr  mesh_geometry_ptr_;

        void init();
        
        /* Default constructor */
        LeafNodeMesh(const std::string &name);
        /* Copy constructor */
        LeafNodeMesh(const LeafNodeMesh& other);
        LeafNodeMesh(const std::string &name,const osgVector4 & color);
        
        /** Initialize weak_ptr */
        void initWeakPtr (LeafNodeMeshWeakPtr other_weak_ptr);

        
    protected:
    public:
        /** Static method which create a new LeafNodeCollada
         */
        static LeafNodeMeshPtr_t create(const std:: string &name);
    static LeafNodeMeshPtr_t create(const std::string &name, const osgVector4& color);
        
        /** Static method for creating a clone of LeafNodeMesh other
         */
        static LeafNodeMeshPtr_t createCopy(LeafNodeMeshPtr_t other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeMeshPtr_t clone(void) const;
        

        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual LeafNodeMeshPtr_t copy() const { return clone(); }
        
        /** Return a shared pointer of the current object
         */
        LeafNodeMeshPtr_t self(void) const;
        
	void setColor(const osgVector4& color_diffuse);

        void setColor(const osgVector4& color_diffuse,
		      const osgVector4& color_specular,
		      const osgVector4& color_emissive);   

        void setTexture(const std::string& image_path);     

	virtual void setAlpha(const float& alpha);
	virtual osg::ref_ptr<osg::Node> getOsgNode() const;

        SCENE_VIEWER_ACCEPT_VISITOR;

	/** Add geometric data */
	/** Set the vertices of the mesh */
	void setVertexArray(osg::Vec3ArrayRefPtr arrayOfVertices);
	
	/** Add a primitive set to the mesh */
	void addPrimitiveSet(osg::DrawElementsUInt * aSetOfColors);
	
        void setColorBinding(osg::Geometry::AttributeBinding aColorBinding);
	/** Add colors */
	void setColorArray(osg::Vec4ArrayRefPtr colors);

        /** Add normals */
        void setNormalArray(osg::Vec3ArrayRefPtr normals);
        void setNormalBinding(osg::Geometry::AttributeBinding aNormalBinding);
        /** Destructor */
        virtual ~LeafNodeMesh();
        
    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LEAFNODECOLLADA_HH */
