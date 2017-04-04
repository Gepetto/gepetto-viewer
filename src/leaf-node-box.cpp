//
//  leaf-node-box.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-box.h>
#include <stdio.h>

namespace graphics {
    
    /* Declaration of private function members */

    void LeafNodeBox::init ()
    {
        /* Create sphere object */
        box_ptr_ = new ::osg::Box();
        
        /* Set ShapeDrawable */
        shape_drawable_ptr_ = new ::osg::ShapeDrawable(box_ptr_);
        
        /* Create Geode for adding ShapeDrawable */
        geode_ptr_ = new ::osg::Geode();
        geode_ptr_->addDrawable(shape_drawable_ptr_);

        addProperty(Vector3Property::create("HalfLength",
              Vector3Property::Getter_t(boost::bind(&osg::Box::getHalfLengths, box_ptr_.get())),
              Vector3Property::Setter_t(boost::bind(&osg::Box::setHalfLengths, box_ptr_.get(), _1))));
	
        /* Create PositionAttitudeTransform */
        this->asQueue()->addChild(geode_ptr_);
        
        /* Allow transparency */
        geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);      
    }
    
    LeafNodeBox::LeafNodeBox (const std::string &name, const osgVector3& half_axis) :
        Node(name)
    {
        init();
        setHalfAxis(half_axis);
        setColor(osgVector4(1.,1.,1.,1.));
    }

    LeafNodeBox::LeafNodeBox (const std::string &name, const osgVector3& half_axis, const osgVector4 &color) :
        Node(name)
    {
        init();
        setHalfAxis(half_axis);
        setColor(color);
    }

    LeafNodeBox::LeafNodeBox (const LeafNodeBox& other) :
        Node(other)
    {
        init();
        setHalfAxis(other.getHalfAxis());
        setColor(other.getColor());
    }
    
    void LeafNodeBox::initWeakPtr (LeafNodeBoxWeakPtr other_weak_ptr )
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */
    
    LeafNodeBoxPtr_t LeafNodeBox::create (const std::string &name, const osgVector3 &half_axis)
    {
        LeafNodeBoxPtr_t shared_ptr(new LeafNodeBox(name, half_axis));
        
        // Add reference to itself
        shared_ptr->initWeakPtr( shared_ptr );
        
        return shared_ptr;
    }

    LeafNodeBoxPtr_t LeafNodeBox::create (const std::string &name, const osgVector3 &half_axis, const osgVector4 &color)
    {
        LeafNodeBoxPtr_t shared_ptr(new LeafNodeBox(name, half_axis, color));

        // Add reference to itself
        shared_ptr->initWeakPtr( shared_ptr );

        return shared_ptr;
    }
    
    LeafNodeBoxPtr_t LeafNodeBox::createCopy (LeafNodeBoxPtr_t other)
    {
        LeafNodeBoxPtr_t shared_ptr(new LeafNodeBox(*other));
        
        // Add reference to itself
        shared_ptr->initWeakPtr( shared_ptr );
        
        return shared_ptr;
    }
    
    /* End of declaration of protected function members */
    
    /* Declaration of public function members */
    
    LeafNodeBoxPtr_t LeafNodeBox::clone (void) const
    {
        return LeafNodeBox::createCopy(weak_ptr_.lock());
    }
    
    LeafNodeBoxPtr_t LeafNodeBox::self (void) const
    {
        return weak_ptr_.lock();
    }
    
    void LeafNodeBox::setHalfAxis (const osgVector3& half_axis)
    {        
        box_ptr_->setHalfLengths(half_axis);
    }
    
    void LeafNodeBox::setColor (const osgVector4 &color)
    {
        shape_drawable_ptr_->setColor(color);
    }

    void LeafNodeBox::setTexture(const std::string& image_path)
    {
      osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
      texture->setDataVariance(osg::Object::DYNAMIC); 
      osg::ref_ptr<osg::Image> image = osgDB::readImageFile(image_path);
      if (!image)
      {
        std::cerr << " couldn't find texture " << image_path << ", quiting." << std::endl;
        return;
      } 
      texture->setImage(image);
      geode_ptr_->getStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
      printf("ptr of %s is %p\n", getID().c_str(), (void *)geode_ptr_.get());
    }

    LeafNodeBox::~LeafNodeBox()
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
