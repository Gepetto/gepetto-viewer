//
//  leaf-node-capsule.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-capsule.h>

namespace gepetto {
namespace viewer {

/* Declaration of private function members */

void LeafNodeCapsule::init() {
  /* Create capsule object */

  capsule_ptr_ = new ::osg::Capsule();

  /* Set ShapeDrawable */
  shape_drawable_ptr_ = new ::osg::ShapeDrawable(capsule_ptr_);

  /* Create Geode for adding ShapeDrawable */
  geode_ptr_ = new osg::Geode();
  geode_ptr_->addDrawable(shape_drawable_ptr_);

  RangedFloatProperty::Ptr_t radiusProp = RangedFloatProperty::create(
      "Radius", this, &LeafNodeCapsule::getRadius, &LeafNodeCapsule::setRadius);
  radiusProp->min = 0.f;
  radiusProp->step = 0.1f;
  radiusProp->adaptiveDecimal = true;
  addProperty(radiusProp);

  RangedFloatProperty::Ptr_t heightProp = RangedFloatProperty::create(
      "Height", this, &LeafNodeCapsule::getHeight, &LeafNodeCapsule::setHeight);
  heightProp->min = 0.f;
  heightProp->step = 0.1f;
  heightProp->adaptiveDecimal = true;
  addProperty(heightProp);

  /* Create PositionAttitudeTransform */
  this->asQueue()->addChild(geode_ptr_);

  /* Allow transparency */
  geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND,
                                             ::osg::StateAttribute::ON);
}

LeafNodeCapsule::LeafNodeCapsule(const std::string& name, const float& radius,
                                 const float& height)
    : NodeDrawable(name) {
  init();
  setRadius(radius);
  setHeight(height);
  setColor(osgVector4(1., 1., 1., 1.));
}

LeafNodeCapsule::LeafNodeCapsule(const std::string& name, const float& radius,
                                 const float& height, const osgVector4& color)
    : NodeDrawable(name) {
  init();
  setRadius(radius);
  setHeight(height);
  setColor(color);
}

LeafNodeCapsule::LeafNodeCapsule(const LeafNodeCapsule& other)
    : NodeDrawable(other) {
  init();
  setRadius(other.getRadius());
  setHeight(other.getHeight());
  setColor(other.getColor());
}

void LeafNodeCapsule::initWeakPtr(LeafNodeCapsuleWeakPtr other_weak_ptr) {
  weak_ptr_ = other_weak_ptr;
}

/* End of declaration of private function members */

/* Declaration of protected function members */

LeafNodeCapsulePtr_t LeafNodeCapsule::create(const std::string& name,
                                             const float& radius,
                                             const float& height) {
  LeafNodeCapsulePtr_t shared_ptr(new LeafNodeCapsule(name, radius, height));

  // Add reference to itself
  shared_ptr->initWeakPtr(shared_ptr);

  return shared_ptr;
}

LeafNodeCapsulePtr_t LeafNodeCapsule::create(const std::string& name,
                                             const float& radius,
                                             const float& height,
                                             const osgVector4& color) {
  LeafNodeCapsulePtr_t shared_ptr(
      new LeafNodeCapsule(name, radius, height, color));

  // Add reference to itself
  shared_ptr->initWeakPtr(shared_ptr);

  return shared_ptr;
}

LeafNodeCapsulePtr_t LeafNodeCapsule::createCopy(
    const LeafNodeCapsulePtr_t& other) {
  LeafNodeCapsulePtr_t shared_ptr(new LeafNodeCapsule(*other));

  // Add reference to itself
  shared_ptr->initWeakPtr(shared_ptr);

  return shared_ptr;
}

/* End of declaration of protected function members */

/* Declaration of public function members */

LeafNodeCapsulePtr_t LeafNodeCapsule::clone(void) const {
  return LeafNodeCapsule::createCopy(weak_ptr_.lock());
}

LeafNodeCapsulePtr_t LeafNodeCapsule::self(void) const {
  return weak_ptr_.lock();
}

void LeafNodeCapsule::setRadius(const float& radius) {
  capsule_ptr_->setRadius(radius);
  redrawShape();
  setDirty();
}

void LeafNodeCapsule::setHeight(const float& height) {
  capsule_ptr_->setHeight(height);
  redrawShape();
}

LeafNodeCapsule::~LeafNodeCapsule() {
  /* Proper deletion of all tree scene */
  geode_ptr_->removeDrawable(shape_drawable_ptr_);
  shape_drawable_ptr_ = NULL;

  this->asQueue()->removeChild(geode_ptr_);
  geode_ptr_ = NULL;

  weak_ptr_.reset();
}

/* End of declaration of public function members */

} /* namespace viewer */

} /* namespace gepetto */
