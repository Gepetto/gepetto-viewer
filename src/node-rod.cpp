//
//  node-rod.cpp
//  gepetto-viewer
//
//  Created by Pierre Fernbach in may 2015.
//  Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//
#include <gepetto/viewer/node-rod.h>

#include <sstream>

#include "log.hh"

namespace gepetto {
namespace viewer {

// ---- protected constructor ---- //

NodeRod::NodeRod(const std::string& name, osgVector4 color, float radius,
                 float totalLength, size_t maxCapsule)
    : Node(name), list_of_capsule_() {
  radius_ = radius;
  totalLength_ = totalLength;
  color_ = osgVector4(color);
  maxCapsule_ = maxCapsule;

  osgVector3 zero;
  // Z -> -X
  osgQuat y_90(M_PI / 2, osgVector3(0, 1, 0));
  for (size_t i = 0; i < maxCapsule; i++) {
    std::stringstream nameCap;
    nameCap << name << "_cap" << i;
    LeafNodeCapsulePtr_t cap = LeafNodeCapsule::create(
        nameCap.str(), radius, (totalLength / ((float)maxCapsule)), color);
    cap->setStaticTransform(zero, y_90);
    list_of_capsule_.push_back(cap);
    this->asQueue()->addChild(cap->asGroup());
  }
}

NodeRod::NodeRod(const NodeRod& other) : Node(other) {
  size_t i;
  radius_ = other.radius();
  totalLength_ = other.totalLength();
  color_ = other.color();
  maxCapsule_ = other.maxCapsule();

  for (i = 0; i < (size_t)other.maxCapsule(); i++) {
    LeafNodeCapsulePtr_t cap = LeafNodeCapsule::createCopy(other.getCapsule(i));
    list_of_capsule_.push_back(cap);
  }
}

void NodeRod::initWeakPtr(NodeRodWeakPtr other_weak_ptr) {
  weak_ptr_ = other_weak_ptr;
}

//------- public :------- //

NodeRodPtr_t NodeRod::create(const std::string& name, osgVector4 color,
                             float radius, float totalLength,
                             short maxCapsule) {
  NodeRodPtr_t shared_ptr(
      new NodeRod(name, color, radius, totalLength, maxCapsule));

  // Add reference to itself
  shared_ptr->initWeakPtr(shared_ptr);

  return shared_ptr;
}

NodeRodPtr_t NodeRod::createCopy(NodeRodPtr_t other) {
  NodeRodPtr_t shared_ptr(new NodeRod(*other));

  // Add reference to itself
  shared_ptr->initWeakPtr(shared_ptr);

  return shared_ptr;
}

NodeRodPtr_t NodeRod::self(void) const { return weak_ptr_.lock(); }

std::string NodeRod::getCapsuleName(size_t i) {
  return list_of_capsule_.at(i)->getID();
}

LeafNodeCapsulePtr_t NodeRod::getCapsule(size_t i) const {
  return list_of_capsule_.at(i);
}

/**
  Applyconfiguration to all the capsules of the rod,
  params is a sequence of maxCapsule parameter, size 8 (length + pos + quat), or
  size 7 (pos + quat) quat : (w,x,y,z)
  */
void NodeRod::applyConfiguration(std::vector<std::vector<value_type> > params) {
  if (params.size() != (size_t)maxCapsule_)
    log() << "Error in rod Applyconfiguration : the size of the sequence must "
             "be the same as the number of capsules"
          << std::endl;

  for (size_t i = 0; i < params.size(); i++) {
    if (params.at(i).size() == 8) {
      list_of_capsule_.at(i)->setHeight(params.at(i)[0]);
      ::osg::Quat quat = ::osg::Quat(params.at(i)[5], params.at(i)[6],
                                     params.at(i)[7], params.at(i)[4]);
      ::osg::Vec3f pos =
          ::osg::Vec3f(params.at(i)[1], params.at(i)[2], params.at(i)[3]);
      list_of_capsule_.at(i)->applyConfiguration(pos, quat);
    } else if (params.at(i).size() == 7) {
      ::osg::Quat quat = ::osg::Quat(params.at(i)[4], params.at(i)[5],
                                     params.at(i)[6], params.at(i)[3]);
      ::osg::Vec3f pos =
          ::osg::Vec3f(params.at(i)[0], params.at(i)[1], params.at(i)[2]);
      list_of_capsule_.at(i)->applyConfiguration(pos, quat);

    } else
      log() << "Parameter must have 7 or 8 elements" << std::endl;
  }
}

} /* namespace viewer */
}  // namespace gepetto
