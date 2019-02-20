//
//  leaf-node-face.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_LEAFNODEFACE_HH
#define GEPETTO_VIEWER_LEAFNODEFACE_HH

#include <gepetto/viewer/node.h>

namespace gepetto {
namespace viewer {

    DEF_CLASS_SMART_PTR(LeafNodeFace)
    
    /** Implementation of the Face GraphicalObject in OSG render */
    class LeafNodeFace : public Node
    {
    private:
        /** Associated weak pointer */
        LeafNodeFaceWeakPtr weak_ptr_;
        
        /** Associated Geometry for face*/
        ::osg::GeometryRefPtr face_ptr_;
        ::osg::Vec3ArrayRefPtr vertices_;
        ::osg::Vec4ArrayRefPtr color_ptr_;

        void init ();

        
        /* Default constructor */
        LeafNodeFace(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3);
        LeafNodeFace(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector4& color);
        LeafNodeFace(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector3& point4);
        LeafNodeFace(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector3& point4, const osgVector4& color);

        /* Copy constructor */
        LeafNodeFace(const LeafNodeFace& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr(LeafNodeFaceWeakPtr other_weak_ptr);

    protected:
    public:
        void addVertex (const osgVector3& vertex);
        /** Static method which create a new box defined by the half_axis vector
         */
        static LeafNodeFacePtr_t create(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3);
        static LeafNodeFacePtr_t create(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector4& color);
        static LeafNodeFacePtr_t create(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector3& point4);
        static LeafNodeFacePtr_t create(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector3& point4, const osgVector4& color);
        
        /** Static method for creating a clone of box other with the copy constructor
         */
        static LeafNodeFacePtr_t createCopy(LeafNodeFacePtr_t other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeFacePtr_t clone(void) const;
        
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual NodePtr_t copy() const { return clone(); }
        
        /** Return a shared pointer of the current object
         */
        LeafNodeFacePtr_t self(void) const;

        // set color
        void setColor (const osgVector4& color);

        void setTexture(const std::string& image_path);

	/// Get number of vertices
	std::size_t nbVertices () const { return vertices_->size ();}

        SCENE_VIEWER_ACCEPT_VISITOR;
        
        /** Destructor */
        virtual ~LeafNodeFace();
        
    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LEAFNODEFACE_HH */
