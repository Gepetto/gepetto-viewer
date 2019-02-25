//
//  leaf-node-light.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-light.h>

#include <osgDB/ReadFile>

namespace graphics {
    
    /* Declaration of private function members */

    int LeafNodeLight::uniqueLightNumber = 1;

    void LeafNodeLight::init()
    {
        /* Create light object */
        ::osg::LightRefPtr light = new ::osg::Light();
        light->setLightNum (uniqueLightNumber++);
        light->setPosition(osgVector4(0.0, 0.0, 0.0, 1.0));
        light->setDiffuse(osgVector4(0,0,0,1));
        light->setSpecular(osgVector4(1.0, 1.0, 1.0, 1.0));
        light->setAmbient( osgVector4(0.0, 0.0, 0.0, 1.0));
        light_ptr_ = new ::osg::LightSource();
        light_ptr_->setLight (light);

        addProperty(Vector4Property::create("ColorSpecular",
              Vector4Property::getterFromMemberFunction(light.get(), &osg::Light::getSpecular),
              Vector4Property::setterFromMemberFunction(light.get(), &osg::Light::setSpecular)));
        addProperty(Vector4Property::create("ColorAmbient",
              Vector4Property::getterFromMemberFunction(light.get(), &osg::Light::getAmbient),
              Vector4Property::setterFromMemberFunction(light.get(), &osg::Light::setAmbient)));
        addProperty(Vector4Property::create("ColorDiffuse",
              Vector4Property::getterFromMemberFunction(light.get(), &osg::Light::getDiffuse),
              Vector4Property::setterFromMemberFunction(light.get(), &osg::Light::setDiffuse)));

        /* Create PositionAttitudeTransform */
        this->asQueue()->addChild(light_ptr_);
    }

    LeafNodeLight::LeafNodeLight(const std::string& name, const float& radius) :
        LeafNodeSphere(name, radius)
    {
        init();
        setColor(osgVector4(1.,1.,1.,1.));
    }

    LeafNodeLight::LeafNodeLight(const std::string& name, const float& radius, const osgVector4& color) :
        LeafNodeSphere(name, radius, color)
    {
        init();
        setColor(color);
    }

    LeafNodeLight::LeafNodeLight(const std::string& name, const LeafNodeLight& other) :
        LeafNodeSphere(name, other)
    {
        init();
        setColor(other.getColor());
    }

    void LeafNodeLight::initWeakPtr(LeafNodeLightWeakPtr other_weak_ptr)
    {
        LeafNodeSphere::initWeakPtr(other_weak_ptr);
        weak_ptr_ = other_weak_ptr;
    }

    /* End of declaration of private function members */

    /* Declaration of protected function members */

    LeafNodeLightPtr_t LeafNodeLight::create(const std::string& name, const float &radius)
    {
        LeafNodeLightPtr_t shared_ptr(new LeafNodeLight(name, radius));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    LeafNodeLightPtr_t LeafNodeLight::create(const std::string& name, const float &radius, const osgVector4& color)
    {
        LeafNodeLightPtr_t shared_ptr(new LeafNodeLight(name, radius, color));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    LeafNodeLightPtr_t LeafNodeLight::createCopy(LeafNodeLightPtr_t other)
    {
        LeafNodeLightPtr_t shared_ptr(new LeafNodeLight(*other));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    /* End of declaration of protected function members */

    /* Declaration of public function members */

    LeafNodeLightPtr_t LeafNodeLight::clone(void) const
    {
        return LeafNodeLight::createCopy(weak_ptr_.lock());
    }

    LeafNodeLightPtr_t LeafNodeLight::self(void) const
    {
        return weak_ptr_.lock();
    }

    void LeafNodeLight::setColor (const osgVector4& color)
    {
        light_ptr_->getLight ()->setDiffuse (color);
        ((LeafNodeSphere*)this)->setColor (color);
        setDirty();
    }

    void LeafNodeLight::setRoot (GroupNodePtr_t root)
    {
      light_ptr_->setLocalStateSetModes (::osg::StateAttribute::ON);
      light_ptr_->setStateSetModes (*root->getOrCreateRootStateSet (), ::osg::StateAttribute::ON);
      setDirty();
    }

    LeafNodeLight::~LeafNodeLight()
    {
        this->asQueue()->removeChild(geode_ptr_);
        geode_ptr_ = NULL;

        weak_ptr_.reset();
    }

    /* End of declaration of public function members */

} /* namespace graphics */
