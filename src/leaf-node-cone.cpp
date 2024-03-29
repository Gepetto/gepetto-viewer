//
//  leaf-node-cone.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-cone.h>

namespace gepetto {
namespace viewer {

/* Declaration of private function members */

void LeafNodeCone::init() {
  /* Create cone object */
  cone_ptr_ = new osg::Cone();

  /* Set ShapeDrawable */
  shape_drawable_ptr_ = new osg::ShapeDrawable(cone_ptr_);

  /* Create Geode for adding ShapeDrawable */
  geode_ptr_ = new osg::Geode();
  geode_ptr_->addDrawable(shape_drawable_ptr_);

  /* Create PositionAttitudeTransform */
  this->asQueue()->addChild(geode_ptr_);

  /* Allow transparency */
  geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND,
                                             ::osg::StateAttribute::ON);
}

LeafNodeCone::LeafNodeCone(const std::string& name, const float& radius,
                           const float& height)
    : NodeDrawable(name) {
  init();
  setRadius(radius);
  setHeight(height);
  setColor(osgVector4(1., 1., 1., 1.));
}

LeafNodeCone::LeafNodeCone(const std::string& name, const float& radius,
                           const float& height, const osgVector4& color)
    : NodeDrawable(name) {
  init();
  setRadius(radius);
  setHeight(height);
  setColor(color);
}
LeafNodeCone::LeafNodeCone(const LeafNodeCone& other) : NodeDrawable(other) {
  init();
  setRadius(other.getRadius());
  setHeight(other.getHeight());
  setColor(other.getColor());
}

void LeafNodeCone::initWeakPtr(LeafNodeConeWeakPtr other_weak_ptr) {
  weak_ptr_ = other_weak_ptr;
}

/* End of declaration of private function members */

/* Declaration of protected function members */

LeafNodeConePtr_t LeafNodeCone::create(const std::string& name,
                                       const float& radius,
                                       const float& height) {
  LeafNodeConePtr_t shared_ptr(new LeafNodeCone(name, radius, height));

  // Add reference to itself
  shared_ptr->initWeakPtr(shared_ptr);

  return shared_ptr;
}

LeafNodeConePtr_t LeafNodeCone::create(const std::string& name,
                                       const float& radius, const float& height,
                                       const osgVector4& color) {
  LeafNodeConePtr_t shared_ptr(new LeafNodeCone(name, radius, height, color));

  // Add reference to itself
  shared_ptr->initWeakPtr(shared_ptr);

  return shared_ptr;
}

LeafNodeConePtr_t LeafNodeCone::createCopy(LeafNodeConePtr_t other) {
  LeafNodeConePtr_t shared_ptr(new LeafNodeCone(*other));

  // Add reference to itself
  shared_ptr->initWeakPtr(shared_ptr);

  return shared_ptr;
}

/* End of declaration of protected function members */

/* Declaration of public function members */

LeafNodeConePtr_t LeafNodeCone::clone(void) const {
  return LeafNodeCone::createCopy(weak_ptr_.lock());
}

LeafNodeConePtr_t LeafNodeCone::self(void) const { return weak_ptr_.lock(); }

void LeafNodeCone::setRadius(const float& radius) {
  cone_ptr_->setRadius(radius);
  redrawShape();
}

void LeafNodeCone::setHeight(const float& height) {
  cone_ptr_->setHeight(height);
  redrawShape();
}

LeafNodeCone::~LeafNodeCone() {
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
