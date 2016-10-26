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

	std::cout << "Creating face\n";
        
        /* Create Geode for adding geometry */
        geode_ptr_ = new osg::Geode();
        geode_ptr_->addDrawable(face_ptr_);
        
        vertices_ = new ::osg::Vec3Array();
        face_ptr_->setVertexArray( vertices_ );
        
        /* Define the face color */
        color_ptr_ = new ::osg::Vec4Array();
        color_ptr_->push_back(osgVector4(1.,1.,1.,1.));
        face_ptr_->setColorArray(color_ptr_.get());
        face_ptr_->setColorBinding(::osg::Geometry::BIND_OVERALL);
        
        /* Allow transparency */
        geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);

        /* Add geode to the queue */
        this->asQueue()->addChild(geode_ptr_);
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
        face_ptr_->addPrimitiveSet
	  (new ::osg::DrawArrays (GL_POLYGON, 0,(GLsizei) vertices_->size ()));
        //osgUtil::Tessellator::retessellatePolygons( *(face_ptr_.get()) );
        osgUtil::SmoothingVisitor::smooth( *(face_ptr_.get()) );
    }
    
    void LeafNodeFace::setColor(const osgVector4& color)
    {        
        color_ptr_->at(0) = color;
    }

    void LeafNodeFace::setTexture(const std::string& image_path)
    {
      osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
      // Do not resize image to closest power of two values for width and height
      texture->setResizeNonPowerOfTwoHint (false);
      // Disable interpolation between pixels.
      texture->setFilter (osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
      osg::ref_ptr<osg::Image> image = osgDB::readImageFile(image_path);
      if (!image)
      {
        std::cout << " couldn't find texture, quiting." << std::endl;
        return;
      } 
      texture->setImage(image);
      osg::Vec2Array* texcoords = new osg::Vec2Array(4);
      (*texcoords)[0].set(0.00f,0.0f); // texture coord for vertex 0
      (*texcoords)[1].set(1.00f,0.0f); // texture coord for vertex 1
      (*texcoords)[2].set(1.00f,1.0f); // texture coord for vertex 2
      (*texcoords)[3].set(0.00f,1.0f); // texture coord for vertex 3
      face_ptr_->setTexCoordArray(0,texcoords);

      geode_ptr_->getStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
    }
    
    LeafNodeFace::~LeafNodeFace ()
    {
        /* Proper deletion of all tree scene */
        geode_ptr_->removeDrawable(face_ptr_);
        face_ptr_ = NULL;
        geode_ptr_ = NULL;
        
        weak_ptr_.reset();
    }
    
    /* End of declaration of public function members */
    
} /* namespace graphics */
