//
//  leaf-node-capsule.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-capsule.h>


namespace graphics {
    
    /* Declaration of private function members */
    
    void LeafNodeCapsule::init ()
    {
        auto_transform_ptr_ = new ::osg::AutoTransform;
        /* Create capsule object */

        capsule_ptr_ = new ::osg::Capsule ();

        /* Set ShapeDrawable */
        shape_drawable_ptr_ = new ::osg::ShapeDrawable(capsule_ptr_);
        
        /* Create Geode for adding ShapeDrawable */
        geode_ptr_ = new osg::Geode ();
        geode_ptr_->addDrawable (shape_drawable_ptr_);
        
        addProperty(FloatProperty::create("Radius",
              FloatProperty::getterFromMemberFunction(this, &LeafNodeCapsule::getRadius),
              FloatProperty::setterFromMemberFunction(this, &LeafNodeCapsule::setRadius)));
        addProperty(FloatProperty::create("Height",
              FloatProperty::getterFromMemberFunction(this, &LeafNodeCapsule::getHeight),
              FloatProperty::setterFromMemberFunction(this, &LeafNodeCapsule::setHeight)));

        auto_transform_ptr_->addChild(geode_ptr_);
        /* Create PositionAttitudeTransform */
        this->asQueue()->addChild (auto_transform_ptr_);
        
        /* Allow transparency */
        geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);
    }
    
    LeafNodeCapsule::LeafNodeCapsule (const std::string& name, const float& radius, const float &height) :
        Node(name)
    {
        init();
        setRadius(radius);
        setHeight(height);
        setColor(osgVector4(1.,1.,1.,1.));
    }

    LeafNodeCapsule::LeafNodeCapsule (const std::string& name, const float& radius, const float &height, const osgVector4& color) :
        Node(name)
    {
        init();
        setRadius(radius);
        setHeight(height);
        setColor(color);
    }

    LeafNodeCapsule::LeafNodeCapsule (const LeafNodeCapsule& other) :
        Node(other)
    {
        init();
        setRadius(other.getRadius());
        setHeight(other.getHeight());
        setColor(other.getColor());
    }
    
    void LeafNodeCapsule::initWeakPtr (LeafNodeCapsuleWeakPtr other_weak_ptr)
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */
    
    LeafNodeCapsulePtr_t LeafNodeCapsule::create (const std::string& name, const float &radius, const float &height)
    {
        LeafNodeCapsulePtr_t shared_ptr (new LeafNodeCapsule(name, radius, height));
        
        // Add reference to itself
        shared_ptr->initWeakPtr (shared_ptr);
        
        return shared_ptr;
    }
    
    LeafNodeCapsulePtr_t LeafNodeCapsule::create (const std::string& name, const float &radius, const float &height, const osgVector4& color)
    {
        LeafNodeCapsulePtr_t shared_ptr (new LeafNodeCapsule(name, radius, height, color));

        // Add reference to itself
        shared_ptr->initWeakPtr (shared_ptr);

        return shared_ptr;
    }

    LeafNodeCapsulePtr_t LeafNodeCapsule::createCopy (const LeafNodeCapsulePtr_t& other)
    {
        LeafNodeCapsulePtr_t shared_ptr (new LeafNodeCapsule(*other));
        
        // Add reference to itself
        shared_ptr->initWeakPtr (shared_ptr);
        
        return shared_ptr;
    }
    
    /* End of declaration of protected function members */
    
    /* Declaration of public function members */
    
    LeafNodeCapsulePtr_t LeafNodeCapsule::clone (void) const
    {
        return LeafNodeCapsule::createCopy(weak_ptr_.lock());
    }
    
    LeafNodeCapsulePtr_t LeafNodeCapsule::self (void) const
    {
        return weak_ptr_.lock ();
    }
    
    void LeafNodeCapsule::setRadius (const float& radius)
    {
        capsule_ptr_->setRadius(radius);
    }
    
    void LeafNodeCapsule::setHeight (const float& height)
    {
        capsule_ptr_->setHeight(height);
    }

    void LeafNodeCapsule::resize(float height){
      if(height != getHeight()){ // avoid useless resize
        osgVector4 color = shape_drawable_ptr_->getColor();
        geode_ptr_->removeDrawable(shape_drawable_ptr_);
        setHeight(height);
        shape_drawable_ptr_ = new ::osg::ShapeDrawable(capsule_ptr_);
        shape_drawable_ptr_->setColor(color);
        geode_ptr_->addDrawable(shape_drawable_ptr_);
        }
    }
    
    void LeafNodeCapsule::setColor (const osgVector4& color)
    {
        shape_drawable_ptr_->setColor(color);
    }

    // reimplmented from Node : use the mathematical representation instead of OSG representation :
    //( origin in the extremity and not in the center, length on the X+ and not Z+)
    // if size <0, display it on the opposite extremity
    void LeafNodeCapsule::addLandmark(const float& size)
    {
      ::osg::GeometryRefPtr geom_ptr = new ::osg::Geometry();

      /* Define points of the beam */
      ::osg::Vec3ArrayRefPtr points_ptr = new ::osg::Vec3Array(6);
        float offset;
        float absSize = (float) fabs(size);
        if(size<0){
            offset = getHeight()/2;
        }else
            offset = - getHeight()/2;

      points_ptr->at(0) = osgVector3(0.,0.,offset);
      points_ptr->at(1) = osgVector3(0.,0.,absSize+offset);
      points_ptr->at(2) = osgVector3(0.,0.,offset);
      points_ptr->at(3) = osgVector3(0.,absSize,offset);
      points_ptr->at(4) = osgVector3(0.,0.,offset);
      points_ptr->at(5) = osgVector3(-absSize,0.,offset);


      /* Define the color */
      ::osg::Vec4ArrayRefPtr color_ptr = new ::osg::Vec4Array(3);
      color_ptr->at(0) = osgVector4(1.,0.,0.,1.);
      color_ptr->at(1) = osgVector4(0.,1.,0.,1.);
      color_ptr->at(2) = osgVector4(0.,0.,1.,1.);

      geom_ptr->setVertexArray(points_ptr.get());
      geom_ptr->setColorArray(color_ptr.get());
      geom_ptr->setColorBinding(::osg::Geometry::BIND_PER_PRIMITIVE_SET);
      geom_ptr->addPrimitiveSet(new osg::DrawArrays(GL_LINES,0,2));
      geom_ptr->addPrimitiveSet(new osg::DrawArrays(GL_LINES,2,2));
      geom_ptr->addPrimitiveSet(new osg::DrawArrays(GL_LINES,4,2));

      landmark_geode_ptr_ = new osg::Geode();
      landmark_geode_ptr_->addDrawable(geom_ptr);

      //set Landmark as ALWAYS ON TOP
      landmark_geode_ptr_->setNodeMask(0xffffffff);
      landmark_geode_ptr_->getOrCreateStateSet()->setRenderBinDetails(INT_MAX, "DepthSortedBin");
      landmark_geode_ptr_->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
      landmark_geode_ptr_->getOrCreateStateSet()->setMode(GL_CULL_FACE, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED );
      landmark_geode_ptr_->getOrCreateStateSet()->setMode(GL_LIGHTING, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
      this->asQueue()->addChild(landmark_geode_ptr_);
    }

    void LeafNodeCapsule::setTexture(const std::string& image_path)
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

    LeafNodeCapsule::~LeafNodeCapsule ()
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
