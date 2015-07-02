//
//  leaf-node-arrow.cpp
//  gepetto-viewer
//
//  Created by Pierre Fernbach in june 2015.
//  Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-arrow.h>

#include <osgUtil/SmoothingVisitor>
#include <osg/CullFace>
#include <hpp/util/debug.hh>

namespace graphics {

    /* Declaration of private function members */

    void LeafNodeArrow::init ()
    {


        /* Create Geode for adding ShapeDrawable */
        geode_ptr_ = new osg::Geode();

         /* create the axis : */
        float radiusCyl = getRadius();
        float lengthCyl = getSize();



        /* Create cylinder */
        ::osg::CylinderRefPtr cylinder_shape_x_ptr = new ::osg::Cylinder();
        cylinder_shape_x_ptr->set(osgVector3(lengthCyl/2.,0.,0.) , radiusCyl ,lengthCyl);
        cylinder_shape_x_ptr->setRotation(osgQuat( 0. , ::osg::X_AXIS , M_PI_2 , ::osg::Y_AXIS , 0. , ::osg::Z_AXIS ));
        /* Create cone */
        ::osg::ConeRefPtr cone_shape_x_ptr = new ::osg::Cone();
        cone_shape_x_ptr->set(osgVector3(lengthCyl+getRadius(),0.,0.) , 2. * radiusCyl , 4. * radiusCyl );
        cone_shape_x_ptr->setRotation(osgQuat( 0. , ::osg::X_AXIS , M_PI_2 , ::osg::Y_AXIS , 0. , ::osg::Z_AXIS ));

        /* create drawable and add them to geode */
        cylinder_drawable_ = new ::osg::ShapeDrawable( cylinder_shape_x_ptr );
        cone_drawable_ = new ::osg::ShapeDrawable( cone_shape_x_ptr );

        geode_ptr_->addDrawable(cylinder_drawable_);
        geode_ptr_->addDrawable(cone_drawable_);



        /* Create PositionAttitudeTransform */
        this->asQueue()->addChild(geode_ptr_);

        /* Allow transparency */
        geode_ptr_->getOrCreateStateSet()->setRenderBinDetails(10, "DepthSortedBin");
        geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);;
    }


    LeafNodeArrow::LeafNodeArrow(const std::string& name, const osgVector4& color, float radius, float size):
        Node(name)
    {

        setRadius(radius);
        setSize(size);

        init();
        setColor(color);
    }

    LeafNodeArrow::LeafNodeArrow(const LeafNodeArrow& other) :
        Node(other.getID())
    {
        init();
      //TODO
    }

    void LeafNodeArrow::initWeakPtr(LeafNodeArrowWeakPtr other_weak_ptr)
    {
        weak_ptr_ = other_weak_ptr;
    }

    /* End of declaration of private function members */

    /* Declaration of protected function members */

    LeafNodeArrowPtr_t LeafNodeArrow::create (const std::string& name , const osgVector4& color ,float radiusCenter, float size)
    {
        LeafNodeArrowPtr_t shared_ptr(new LeafNodeArrow(name, color, radiusCenter,size));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    LeafNodeArrowPtr_t LeafNodeArrow::create (const std::string& name, const osgVector4& color, float radiusCenter)
    {
        //const float& size = new float;
        //*size = 0;
        LeafNodeArrowPtr_t shared_ptr(new LeafNodeArrow(name, color, radiusCenter,0));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    LeafNodeArrowPtr_t LeafNodeArrow::createCopy (LeafNodeArrowPtr_t other)
    {
        LeafNodeArrowPtr_t shared_ptr(new LeafNodeArrow(*other));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    /* End of declaration of protected function members */

    /* Declaration of public function members */

    LeafNodeArrowPtr_t LeafNodeArrow::clone (void) const
    {
        return LeafNodeArrow::createCopy(weak_ptr_.lock());
    }

    LeafNodeArrowPtr_t LeafNodeArrow::self (void) const
    {
        return weak_ptr_.lock();
    }


    void LeafNodeArrow::setTexture(const std::string& image_path)
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

    void LeafNodeArrow::setRadius (const float& radius)
    {
        radius_=radius;
    }

    float LeafNodeArrow::getRadius() const
    {
        return radius_;
    }

    void LeafNodeArrow::setSize(const float& size){
        size_ = size;
    }

    float LeafNodeArrow::getSize() const
    {
        return size_;
    }

    void LeafNodeArrow::setColor (const osgVector4& color)
    {
        cylinder_drawable_->setColor(color);
        cone_drawable_->setColor(color);
        color_ = color;
    }

    void LeafNodeArrow::resize(float radius,float length){
      if(length != getSize() || radius != getRadius()){ // avoid useless resize
        osgVector4 color = cylinder_drawable_->getColor();
        geode_ptr_->removeDrawable(cylinder_drawable_);
        geode_ptr_->removeDrawable(cone_drawable_);
        setSize(length);
        setRadius(radius);


        /* Create cylinder */
        ::osg::CylinderRefPtr cylinder_shape_x_ptr = new ::osg::Cylinder();
        cylinder_shape_x_ptr->set(osgVector3(getSize()/2. + getRadius(),0.,0.) , getRadius() ,getSize());
        cylinder_shape_x_ptr->setRotation(osgQuat( 0. , ::osg::X_AXIS , M_PI_2 , ::osg::Y_AXIS , 0. , ::osg::Z_AXIS ));
        /* Create cone */
        ::osg::ConeRefPtr cone_shape_x_ptr = new ::osg::Cone();
        cone_shape_x_ptr->set(osgVector3(getSize()+getRadius(),0.,0.) , 2. * getRadius() , 4. * getRadius() );
        cone_shape_x_ptr->setRotation(osgQuat( 0. , ::osg::X_AXIS , M_PI_2 , ::osg::Y_AXIS , 0. , ::osg::Z_AXIS ));

        /* create drawable and add them to geode */
        cylinder_drawable_ = new ::osg::ShapeDrawable( cylinder_shape_x_ptr );
        cone_drawable_ = new ::osg::ShapeDrawable( cone_shape_x_ptr );
        cylinder_drawable_->setColor(color);
        cone_drawable_->setColor(color);

        geode_ptr_->addDrawable(cylinder_drawable_);
        geode_ptr_->addDrawable(cone_drawable_);

        }
    }

    LeafNodeArrow::~LeafNodeArrow ()
    {
        /* Proper deletion of all tree scene */

        geode_ptr_->removeDrawable(cylinder_drawable_);
        geode_ptr_->removeDrawable(cone_drawable_);

        geode_ptr_ = NULL;
        cylinder_drawable_ = NULL;
        cone_drawable_ = NULL;

        weak_ptr_.reset();
    }

    /* End of declaration of public function members */

} /* namespace graphics */

