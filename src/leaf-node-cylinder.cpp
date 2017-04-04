//
//  leaf-node-cylinder.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-cylinder.h>

namespace graphics {
    
    /* Declaration of private function members */
    
    void LeafNodeCylinder::init ()
    {
        /* Create cylinder object */
        cylinder_ptr_ = new ::osg::Cylinder ();
        
        /* Set ShapeDrawable */
        shape_drawable_ptr_ = new ::osg::ShapeDrawable(cylinder_ptr_);
        
        /* Create Geode for adding ShapeDrawable */
        geode_ptr_ = new osg::Geode ();
        geode_ptr_->addDrawable (shape_drawable_ptr_);
        
        addProperty(FloatProperty::create("Radius",
              FloatProperty::getterFromMemberFunction(this, &LeafNodeCylinder::getRadius),
              FloatProperty::setterFromMemberFunction(this, &LeafNodeCylinder::setRadius)));
        addProperty(FloatProperty::create("Height",
              FloatProperty::getterFromMemberFunction(this, &LeafNodeCylinder::getHeight),
              FloatProperty::setterFromMemberFunction(this, &LeafNodeCylinder::setHeight)));

        /* Create PositionAttitudeTransform */
        this->asQueue()->addChild (geode_ptr_);
        
        /* Allow transparency */
        geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);
    }
    
    LeafNodeCylinder::LeafNodeCylinder (const std::string &name, const float &radius, const float &height) :
      Node (name)
    {
        init();
        setRadius(radius);
        setHeight(height);
        setColor(osgVector4(1.,1.,1.,1.));
    }

    LeafNodeCylinder::LeafNodeCylinder (const std::string &name, const float &radius, const float &height, const osgVector4 &color) :
      Node (name)
    {
        init();
        setRadius(radius);
        setHeight(height);
        setColor(color);
    }
    
    LeafNodeCylinder::LeafNodeCylinder (const LeafNodeCylinder& other) :
      Node (other)
    {
        init();
        setRadius(other.getRadius());
        setHeight(other.getHeight());
        setColor(other.getColor());
    }
    
    void LeafNodeCylinder::initWeakPtr (LeafNodeCylinderWeakPtr other_weak_ptr)
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */
    
    LeafNodeCylinderPtr_t LeafNodeCylinder::create (const std::string &name, const float &radius, const float &height)
    {
        LeafNodeCylinderPtr_t shared_ptr (new LeafNodeCylinder(name, radius, height));
        
        // Add reference to itself
        shared_ptr->initWeakPtr (shared_ptr);
        
        return shared_ptr;
    }


    LeafNodeCylinderPtr_t LeafNodeCylinder::create (const std::string &name, const float &radius, const float &height, const osgVector4 &color)
    {
        LeafNodeCylinderPtr_t shared_ptr (new LeafNodeCylinder(name, radius, height, color));

        // Add reference to itself
        shared_ptr->initWeakPtr (shared_ptr);

        return shared_ptr;
    }
    
    LeafNodeCylinderPtr_t LeafNodeCylinder::createCopy (LeafNodeCylinderPtr_t other)
    {
        LeafNodeCylinderPtr_t shared_ptr (new LeafNodeCylinder(*other));
        
        // Add reference to itself
        shared_ptr->initWeakPtr (shared_ptr);
        
        return shared_ptr;
    }
    
    /* End of declaration of protected function members */
    
    /* Declaration of public function members */
    
    LeafNodeCylinderPtr_t LeafNodeCylinder::clone (void) const
    {
        return LeafNodeCylinder::createCopy(weak_ptr_.lock());
    }
    
    LeafNodeCylinderPtr_t LeafNodeCylinder::self (void) const
    {
        return weak_ptr_.lock ();
    }
    
    void LeafNodeCylinder::setRadius (const float& radius)
    {        
        cylinder_ptr_->setRadius(radius);
    }
    
    void LeafNodeCylinder::setHeight (const float& height)
    {        
        cylinder_ptr_->setHeight(height);
    }
    
    void LeafNodeCylinder::setColor (const osgVector4& color)
    {
        shape_drawable_ptr_->setColor(color);
    }

    void LeafNodeCylinder::setTexture(const std::string& image_path)
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
    
    LeafNodeCylinder::~LeafNodeCylinder ()
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
