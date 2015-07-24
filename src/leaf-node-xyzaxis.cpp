//
//  leaf-node-xyz.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert and Pierre Fernbach in april 2015.
//  Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-xyzaxis.h>

#include <osgUtil/SmoothingVisitor>
#include <osg/CullFace>

namespace graphics {

    /* Declaration of private function members */

    void LeafNodeXYZAxis::init ()
    {
        /* Create sphere object */
        sphere_ptr_ = new ::osg::Sphere();
        sphere_ptr_->setRadius(getRadius());
        /* Set ShapeDrawable */
        sphere_drawable_ptr_ = new ::osg::ShapeDrawable(sphere_ptr_);

        /* Create Geode for adding ShapeDrawable */
        geode_ptr_ = new osg::Geode();
        geode_ptr_->addDrawable(sphere_drawable_ptr_);
        if(sizeAxis_ > 0){  // optimisation of memory consumption : doesn't create the axis instead of creating axis with size "0"
             /* create the axis : */
            float radiusCyl = (getRadius()/4.f) * getSizeAxis();
            float lengthCyl = (getRadius()*3.f) * getSizeAxis();

            osgVector4 blue = osgVector4(0.f, 0.f, 1.f, 1.f);
            osgVector4 green = osgVector4(0.f, 1.f, 0.f, 1.f);
            osgVector4 red = osgVector4(1.f, 0.f, 0.f, 1.f);


            /* X_AXIS */
            /* Create cylinder */
            ::osg::CylinderRefPtr cylinder_shape_x_ptr = new ::osg::Cylinder();
            cylinder_shape_x_ptr->set(osgVector3(lengthCyl/2.f + getRadius(),0.,0.) , radiusCyl ,lengthCyl);
            cylinder_shape_x_ptr->setRotation(osgQuat( 0. , ::osg::X_AXIS , ::osg::PI_2 , ::osg::Y_AXIS , 0. , ::osg::Z_AXIS ));
            /* Create cone */
            ::osg::ConeRefPtr cone_shape_x_ptr = new ::osg::Cone();
            cone_shape_x_ptr->set(osgVector3(lengthCyl+getRadius(),0.,0.) , 2.f * radiusCyl , 4.f * radiusCyl );
            cone_shape_x_ptr->setRotation(osgQuat( 0. , ::osg::X_AXIS , ::osg::PI_2 , ::osg::Y_AXIS , 0. , ::osg::Z_AXIS ));

            /* create drawable and add them to geode */
            x_cylinder_drawable_ = new ::osg::ShapeDrawable( cylinder_shape_x_ptr );
            x_cone_drawable_ = new ::osg::ShapeDrawable( cone_shape_x_ptr );
            x_cylinder_drawable_->setColor(red);
            x_cone_drawable_->setColor(red);

            geode_ptr_->addDrawable(x_cylinder_drawable_);
            geode_ptr_->addDrawable(x_cone_drawable_);


            /* Y_AXIS */
            /* Create cylinder */
            ::osg::CylinderRefPtr cylinder_shape_y_ptr = new ::osg::Cylinder();
            cylinder_shape_y_ptr->set(osgVector3(0.,lengthCyl/2.f + getRadius(),0.) , radiusCyl ,lengthCyl);
            cylinder_shape_y_ptr->setRotation(osgQuat( -::osg::PI_2 , ::osg::X_AXIS , 0. , ::osg::Y_AXIS , 0. , ::osg::Z_AXIS ));
            /* Create cone */
            ::osg::ConeRefPtr cone_shape_y_ptr = new ::osg::Cone();
            cone_shape_y_ptr->set(osgVector3(0.,lengthCyl + getRadius(),0.) , 2.f * radiusCyl , 4.f * radiusCyl );
            cone_shape_y_ptr->setRotation(osgQuat( -::osg::PI_2 , ::osg::X_AXIS , 0. , ::osg::Y_AXIS , 0. , ::osg::Z_AXIS ));

            /* create drawable and add them to geode */
            y_cylinder_drawable_ = new ::osg::ShapeDrawable( cylinder_shape_y_ptr );
            y_cone_drawable_ = new ::osg::ShapeDrawable( cone_shape_y_ptr );
            y_cylinder_drawable_->setColor(green);
            y_cone_drawable_->setColor(green);

            geode_ptr_->addDrawable(y_cylinder_drawable_);
            geode_ptr_->addDrawable(y_cone_drawable_);


            /* Z_AXIS */
            /* Create cylinder */
            ::osg::CylinderRefPtr cylinder_shape_z_ptr = new ::osg::Cylinder();
            cylinder_shape_z_ptr->set(osgVector3(0.,0.,lengthCyl/2.f + getRadius()) , radiusCyl ,lengthCyl);

            /* Create cone */
            ::osg::ConeRefPtr cone_shape_z_ptr = new ::osg::Cone();
            cone_shape_z_ptr->set(osgVector3(0.,0.,lengthCyl +getRadius()) , 2.f * radiusCyl , 4.f * radiusCyl );


            /* create drawable and add them to geode */
            z_cylinder_drawable_ = new ::osg::ShapeDrawable( cylinder_shape_z_ptr );
            z_cone_drawable_ = new ::osg::ShapeDrawable( cone_shape_z_ptr );
            z_cylinder_drawable_->setColor(blue);
            z_cone_drawable_->setColor(blue);

            geode_ptr_->addDrawable(z_cylinder_drawable_);
            geode_ptr_->addDrawable(z_cone_drawable_);
        } // if radius > 0 : create axis


        /* Create PositionAttitudeTransform */
        this->asQueue()->addChild(geode_ptr_);

        /* Allow transparency */
        geode_ptr_->getOrCreateStateSet()->setRenderBinDetails(10, "DepthSortedBin");
        geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);;
    }


    LeafNodeXYZAxis::LeafNodeXYZAxis(const std::string& name, const osgVector4& color, float radiusCenter, float sizeAxis):
        Node(name)
    {

        setRadius(radiusCenter);
        setSizeAxis(sizeAxis);

        init();
        setColor(color);
    }

    LeafNodeXYZAxis::LeafNodeXYZAxis(const LeafNodeXYZAxis& other) :
        Node(other.getID())
    {
        init();
      //TODO
    }

    void LeafNodeXYZAxis::initWeakPtr(LeafNodeXYZAxisWeakPtr other_weak_ptr)
    {
        weak_ptr_ = other_weak_ptr;
    }

    /* End of declaration of private function members */

    /* Declaration of protected function members */

    LeafNodeXYZAxisPtr_t LeafNodeXYZAxis::create (const std::string& name , const osgVector4& color ,float radiusCenter, float sizeAxis)
    {
        LeafNodeXYZAxisPtr_t shared_ptr(new LeafNodeXYZAxis(name, color, radiusCenter,sizeAxis));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    LeafNodeXYZAxisPtr_t LeafNodeXYZAxis::create (const std::string& name, const osgVector4& color, float radiusCenter)
    {
        //const float& size = new float;
        //*size = 0;
        LeafNodeXYZAxisPtr_t shared_ptr(new LeafNodeXYZAxis(name, color, radiusCenter,0));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    LeafNodeXYZAxisPtr_t LeafNodeXYZAxis::createCopy (LeafNodeXYZAxisPtr_t other)
    {
        LeafNodeXYZAxisPtr_t shared_ptr(new LeafNodeXYZAxis(*other));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    /* End of declaration of protected function members */

    /* Declaration of public function members */

    LeafNodeXYZAxisPtr_t LeafNodeXYZAxis::clone (void) const
    {
        return LeafNodeXYZAxis::createCopy(weak_ptr_.lock());
    }

    LeafNodeXYZAxisPtr_t LeafNodeXYZAxis::self (void) const
    {
        return weak_ptr_.lock();
    }


    void LeafNodeXYZAxis::setTexture(const std::string& image_path)
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

    void LeafNodeXYZAxis::setRadius (const float& radius)
    {
        radius_=radius;
    }

    float LeafNodeXYZAxis::getRadius() const
    {
        return radius_;
    }

    void LeafNodeXYZAxis::setSizeAxis(const float& sizeAxis){
        sizeAxis_ = sizeAxis;
    }

    float LeafNodeXYZAxis::getSizeAxis() const
    {
        return sizeAxis_;
    }

    void LeafNodeXYZAxis::setColor (const osgVector4& color)
    {
        sphere_drawable_ptr_->setColor(color);
    }

    LeafNodeXYZAxis::~LeafNodeXYZAxis ()
    {
        /* Proper deletion of all tree scene */
        geode_ptr_->removeDrawable(sphere_drawable_ptr_);
        geode_ptr_->removeDrawable(x_cylinder_drawable_);
        geode_ptr_->removeDrawable(x_cone_drawable_);
        geode_ptr_->removeDrawable(y_cylinder_drawable_);
        geode_ptr_->removeDrawable(y_cone_drawable_);
        geode_ptr_->removeDrawable(z_cylinder_drawable_);
        geode_ptr_->removeDrawable(z_cone_drawable_);
        geode_ptr_ = NULL;
        x_cylinder_drawable_ = NULL;
        x_cone_drawable_ = NULL;
        y_cylinder_drawable_ = NULL;
        y_cone_drawable_ = NULL;
        z_cylinder_drawable_ = NULL;
        z_cone_drawable_ = NULL;
        weak_ptr_.reset();
    }

    /* End of declaration of public function members */

} /* namespace graphics */

