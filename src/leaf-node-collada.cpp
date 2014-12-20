//
//  leaf-node-collada.cpp
//  gepetto-viewer
//
//  Created by Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <fstream>
#include <ios>
#include <gepetto/viewer/leaf-node-collada.h>

namespace graphics {
    
    /* Declaration of private function members */

    void LeafNodeCollada::init()
    {
        collada_ptr_ = osgDB::readNodeFile(collada_file_path_);
        
        /* Create PositionAttitudeTransform */
        this->asQueue()->addChild(collada_ptr_);
        
        /* Allow transparency */
        collada_ptr_->getOrCreateStateSet()->setRenderBinDetails(10, "DepthSortedBin");
        collada_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);
    }
    
    LeafNodeCollada::LeafNodeCollada(const std::string& name, const std::string& collada_file_path) :
      Node(name), collada_file_path_(collada_file_path)
    {
        init();
    }
    
    LeafNodeCollada::LeafNodeCollada(const LeafNodeCollada& other) :
        Node(other.getID()), collada_file_path_(other.collada_file_path_)
    {
        init();
    }
    
    void LeafNodeCollada::initWeakPtr(LeafNodeColladaWeakPtr other_weak_ptr)
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */
    
    LeafNodeColladaPtr_t LeafNodeCollada::create(const std::string& name, const std::string& collada_file_path)
    {
      std::ifstream infile(collada_file_path.c_str ());
      if (!infile.good()) {
	throw std::ios_base::failure (collada_file_path +
				      std::string (" does not exist."));
      }
      LeafNodeColladaPtr_t shared_ptr(new LeafNodeCollada
				      (name, collada_file_path));
        
      // Add reference to itself
      shared_ptr->initWeakPtr(shared_ptr);
        
      return shared_ptr;
    }
    
    LeafNodeColladaPtr_t LeafNodeCollada::createCopy(LeafNodeColladaPtr_t other)
    {
        LeafNodeColladaPtr_t shared_ptr(new LeafNodeCollada(*other));
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }
    
    /* End of declaration of protected function members */
    
    /* Declaration of public function members */
    
    LeafNodeColladaPtr_t LeafNodeCollada::clone(void) const
    {
        return LeafNodeCollada::createCopy(weak_ptr_.lock());
    }
    
    LeafNodeColladaPtr_t LeafNodeCollada::self(void) const
    {
        return weak_ptr_.lock();
    }
    
    LeafNodeCollada::~LeafNodeCollada()
    {
        /* Proper deletion of all tree scene */
        this->asQueue()->removeChild(collada_ptr_);
        collada_ptr_ = NULL;
        
        weak_ptr_.reset();
    }
    
    /* End of declaration of public function members */
    
} /* namespace graphics */
