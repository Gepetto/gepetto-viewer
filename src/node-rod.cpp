//
//  node-rod.cpp
//  gepetto-viewer
//
//  Created by Pierre Fernbach in may 2015.
//  Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//
#include <gepetto/viewer/node-rod.h>


namespace graphics {


  NodeRod::NodeRod (const std::string& name, osgVector4 color, float radius, float totalLength,int maxCapsule):Node(name), list_of_capsule_()
    {
        radius_ = radius;
        totalLength_ = totalLength;
        color_ = osgVector4(color);
        maxCapsule_=maxCapsule;
        for (int i = 0; i< maxCapsule ; i++){
            std::stringstream nameCap;
            nameCap << name << "_cap"<<i;
            LeafNodeCapsulePtr_t cap = LeafNodeCapsule::create(nameCap.str(),radius,(totalLength/((float)maxCapsule)),color);
            list_of_capsule_.push_back(cap);
            this->asQueue()->addChild(cap->asGroup());
        }
    }


  NodeRod::NodeRod (const NodeRod& other):
      graphics::Node (other)
  {
      size_t i;
      radius_ = other.radius();
      totalLength_ = other.totalLength();
      color_ = other.color();
      maxCapsule_=other.maxCapsule();

      for(i = 0 ; i< other.maxCapsule() ; i++){
          LeafNodeCapsulePtr_t cap = LeafNodeCapsule::createCopy(other.getCapsule(i));
          list_of_capsule_.push_back(cap);
        }
  }

    void NodeRod::initWeakPtr(NodeRodWeakPtr other_weak_ptr)
    {
        weak_ptr_ = other_weak_ptr;
    }




    //------- public :-------


    NodeRodPtr_t NodeRod::create(const std::string& name,osgVector4 color, float radius, float totalLength,short maxCapsule)
    {
        NodeRodPtr_t shared_ptr(new NodeRod(name,color,radius,totalLength,maxCapsule));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    NodeRodPtr_t NodeRod::createCopy(NodeRodPtr_t other)
    {
        NodeRodPtr_t shared_ptr(new NodeRod(*other));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }


    NodeRodPtr_t NodeRod::self(void) const
    {
        return weak_ptr_.lock();
    }




    std::string NodeRod::getCapsuleName(int i)
    {
        return list_of_capsule_.at(i)->getID();
    }

    LeafNodeCapsulePtr_t NodeRod::getCapsule(size_t i) const{
      return list_of_capsule_.at(i);
    }

} // namespace graphics

