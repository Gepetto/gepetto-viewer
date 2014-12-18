//
//  leaf-node-face.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-face.h>

#include <osgUtil/SmoothingVisitor>
#include <osg/CullFace>


namespace graphics {
    
    /* Declaration of private function members */
    
    void LeafNodeFace::init ()
    {
        /* Set face Geometry */
        face_ptr_ = new ::osg::Geometry();
        
        /* Create Geode for adding geometry */
        geode_face_ptr_ = new osg::Geode();
        geode_face_ptr_->addDrawable(face_ptr_);
        
        vertices_ = new ::osg::Vec3Array();
        face_ptr_->setVertexArray( vertices_ );
        
        /* Define the face color */
        color_ptr_ = new ::osg::Vec4Array();
        color_ptr_->push_back(osgVector4(1.,1.,1.,1.));
        face_ptr_->setColorArray(color_ptr_.get());
        face_ptr_->setColorBinding(::osg::Geometry::BIND_OVERALL);
        
        /* Allow transparency */
        geode_face_ptr_->getOrCreateStateSet()->setRenderBinDetails(10, "DepthSortedBin");
        geode_face_ptr_->getStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);

        /* Add geode to the queue */
        this->asQueue()->addChild(geode_face_ptr_);
    }

    LeafNodeFace::LeafNodeFace(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3) :
        Node(name)
    {
        init();
        addVertex(point1);
        addVertex(point2);
        addVertex(point3);
    }

    LeafNodeFace::LeafNodeFace(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector4& color) :
        Node(name)
    {
        init();
        addVertex(point1);
        addVertex(point2);
        addVertex(point3);
        setColor(color);
    }
    
    LeafNodeFace::LeafNodeFace(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector3& point4) :
        Node(name)
    {
        init();
        addVertex(point1);
        addVertex(point2);
        addVertex(point3);
        addVertex(point4);
    }
    LeafNodeFace::LeafNodeFace(const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector3& point4, const osgVector4& color) :
        Node(name)
    {
        init();
        addVertex(point1);
        addVertex(point2);
        addVertex(point3);
        addVertex(point4);
        setColor(color);
    }

    LeafNodeFace::LeafNodeFace(const LeafNodeFace& other) :
        Node(other.getID())
    {
        init();
        for (::osg::Vec3Array::const_iterator i = other.vertices_->begin() ; i != other.vertices_->end() ; i++ )
        {
            addVertex(*i);
        }
    }
    
    void LeafNodeFace::initWeakPtr(LeafNodeFaceWeakPtr other_weak_ptr)
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */
    
    LeafNodeFacePtr_t LeafNodeFace::create (const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3)
    {
        LeafNodeFacePtr_t shared_ptr(new LeafNodeFace(name, point1, point2, point3));
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }
    
    LeafNodeFacePtr_t LeafNodeFace::create (const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector4& color)
    {
        LeafNodeFacePtr_t shared_ptr(new LeafNodeFace(name, point1, point2, point3, color));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    LeafNodeFacePtr_t LeafNodeFace::create (const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector3& point4)
    {
        LeafNodeFacePtr_t shared_ptr(new LeafNodeFace(name, point1, point2, point3, point4));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    LeafNodeFacePtr_t LeafNodeFace::create (const std::string& name, const osgVector3& point1, const osgVector3& point2, const osgVector3& point3, const osgVector3& point4, const osgVector4& color)
    {
        LeafNodeFacePtr_t shared_ptr(new LeafNodeFace(name, point1, point2, point3, point4, color));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }
    LeafNodeFacePtr_t LeafNodeFace::createCopy (LeafNodeFacePtr_t other)
    {
        LeafNodeFacePtr_t shared_ptr(new LeafNodeFace(*other));
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }
    
    /* End of declaration of protected function members */
    
    /* Declaration of public function members */
    
    LeafNodeFacePtr_t LeafNodeFace::clone (void) const
    {
        return LeafNodeFace::createCopy(weak_ptr_.lock());
    }
    
    LeafNodeFacePtr_t LeafNodeFace::self (void) const
    {
        return weak_ptr_.lock();
    }
      
    void LeafNodeFace::addVertex (const osgVector3& vertex)
    {
        vertices_->push_back(vertex);
        face_ptr_->addPrimitiveSet( new ::osg::DrawArrays( GL_POLYGON, 0, vertices_->size() ) );
        //osgUtil::Tessellator::retessellatePolygons( *(face_ptr_.get()) );
        osgUtil::SmoothingVisitor::smooth( *(face_ptr_.get()) );
    }
    
    void LeafNodeFace::setColor(const osgVector4& color)
    {        
        color_ptr_->at(0) = color;
    }
    
    LeafNodeFace::~LeafNodeFace ()
    {
        /* Proper deletion of all tree scene */
        geode_face_ptr_->removeDrawable(face_ptr_);
        face_ptr_ = NULL;
        geode_face_ptr_ = NULL;
        
        weak_ptr_.reset();
    }
    
    /* End of declaration of public function members */
    
} /* namespace graphics */
