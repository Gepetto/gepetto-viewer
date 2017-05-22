//
//  leaf-node-sphere.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-sphere.h>

namespace graphics {
    
    /* Declaration of private function members */

    void LeafNodeSphere::init()
    {
        /* Create sphere object */
        sphere_ptr_ = new ::osg::Sphere();
        
        /* Set ShapeDrawable */
        shape_drawable_ptr_ = new ::osg::ShapeDrawable(sphere_ptr_);
        
        /* Create Geode for adding ShapeDrawable */
        geode_ptr_ = new osg::Geode();
        geode_ptr_->addDrawable(shape_drawable_ptr_);
        
        /* Create PositionAttitudeTransform */
        this->asQueue()->addChild(geode_ptr_);
        
        /* Allow transparency */
        geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);

        addProperty(FloatProperty::create("Radius",
              FloatProperty::getterFromMemberFunction(sphere_ptr_.get(), &osg::Sphere::getRadius),
              FloatProperty::setterFromMemberFunction(sphere_ptr_.get(), &osg::Sphere::setRadius)));
    }
    
    LeafNodeSphere::LeafNodeSphere(const std::string& name, const float& radius) :
        Node(name)
    {
        init();
        setRadius(radius);
        setColor(osgVector4(1.,1.,1.,1.));
    }

    LeafNodeSphere::LeafNodeSphere(const std::string& name, const float& radius, const osgVector4& color) :
        Node(name)
    {
        init();
        setRadius(radius);
        setColor(color);
    }

    LeafNodeSphere::LeafNodeSphere(const std::string& name, const LeafNodeSphere& other) :
        Node(other)
    {
        setID(name);
        init();
        setRadius(other.getRadius());
        setColor(other.getColor());
    }
    
    void LeafNodeSphere::initWeakPtr(LeafNodeSphereWeakPtr other_weak_ptr)
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */
    
    LeafNodeSpherePtr_t LeafNodeSphere::create(const std::string& name, const float &radius)
    {
        LeafNodeSpherePtr_t shared_ptr(new LeafNodeSphere(name, radius));
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }

    LeafNodeSpherePtr_t LeafNodeSphere::create(const std::string& name, const float &radius, const osgVector4& color)
    {
        LeafNodeSpherePtr_t shared_ptr(new LeafNodeSphere(name, radius, color));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    LeafNodeSpherePtr_t LeafNodeSphere::createCopy(LeafNodeSpherePtr_t other)
    {
        LeafNodeSpherePtr_t shared_ptr(new LeafNodeSphere(*other));
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }
    
    /* End of declaration of protected function members */
    
    /* Declaration of public function members */
    
    LeafNodeSpherePtr_t LeafNodeSphere::clone(void) const
    {
        return LeafNodeSphere::createCopy(weak_ptr_.lock());
    }
    
    LeafNodeSpherePtr_t LeafNodeSphere::self(void) const
    {
        return weak_ptr_.lock();
    }
    
    void LeafNodeSphere::setRadius (const float& radius)
    {
        sphere_ptr_->setRadius(radius);
    }
    
    void LeafNodeSphere::setColor (const osgVector4& color)
    {
        shape_drawable_ptr_->setColor(color);
    }
    
    void LeafNodeSphere::setTexture(const std::string& image_path)
    {
      osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
      texture->setDataVariance(osg::Object::DYNAMIC); 
      osg::ref_ptr<osg::Image> image = osgDB::readImageFile(image_path);
      if (!image)
      {
        std::cout << " couldn't find texture, quiting." << std::endl;
        return;
      } 
      texture->setImage(image);
      geode_ptr_->getStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
    }

    LeafNodeSphere::~LeafNodeSphere()
    {
        /* Proper deletion of all tree scene */
        geode_ptr_->removeDrawable(shape_drawable_ptr_);
        shape_drawable_ptr_ = NULL;
        
        this->asQueue()->removeChild(geode_ptr_);
        geode_ptr_ = NULL;
        
        weak_ptr_.reset();
    }
    
    /* End of declaration of public function members */
    
} /* namespace graphics */
