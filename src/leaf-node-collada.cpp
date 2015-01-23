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
osg::ref_ptr<osgDB::Options> option = new osgDB::Options("daeUsePredefinedTextureUnits");
    collada_ptr_ = osgDB::readNodeFile(collada_file_path_, option);
        
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
    
  LeafNodeCollada::LeafNodeCollada(const std::string& name, const std::string& collada_file_path, const osgVector4& color) :
    Node(name), collada_file_path_(collada_file_path)
  {
    init();
    setColor(color);
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

    LeafNodeColladaPtr_t LeafNodeCollada::create(const std::string& name, const std::string& collada_file_path, const osgVector4& color)
  {
    std::ifstream infile(collada_file_path.c_str ());
    if (!infile.good()) {
      throw std::ios_base::failure (collada_file_path +
                                    std::string (" does not exist."));
    }
    LeafNodeColladaPtr_t shared_ptr(new LeafNodeCollada
                                    (name, collada_file_path, color));
    
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

  ::osg::NodeRefPtr LeafNodeCollada::getColladaPtr()
  {
    return collada_ptr_.get();
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
    
  void LeafNodeCollada::setColor(const osgVector4& color)
  {
    //setColor(collada_ptr_,color);
    osg::ref_ptr<osg::Material> mat_ptr (new osg::Material); 
    mat_ptr->setDiffuse(osg::Material::FRONT_AND_BACK,color); 
    collada_ptr_->getStateSet()->setAttribute(mat_ptr.get());    
  }

  /*void LeafNodeCollada::setColor(osg::NodeRefPtr osgNode_ptr,const osgVector4& color)
  {
    osg::Vec4ArrayRefPtr colorArray = new osg::Vec4Array();
    colorArray->push_back(color);
    osg::GeodeRefPtr geode_ptr = osgNode_ptr->asGeode();
    if (geode_ptr) {
      for (unsigned int i = 0 ; i < geode_ptr->getNumDrawables() ; i++) {
      osg::GeometryRefPtr geom_ptr = geode_ptr->getDrawable(i)->asGeometry();
        if (geom_ptr) {
          geom_ptr->setColorArray(colorArray.get());
          geom_ptr->setColorBinding(osg::Geometry::BIND_OVERALL); 
        }
      }
    }
    else {
      osg::GroupRefPtr group_ptr = osgNode_ptr->asGroup();
      if (group_ptr) {
        for (unsigned int i = 0 ; i < group_ptr->getNumChildren() ; i++) {
          setColor(group_ptr->getChild(i),color);
        }
      }
    }
  }*/

  LeafNodeCollada::~LeafNodeCollada()
  {
    /* Proper deletion of all tree scene */
    this->asQueue()->removeChild(collada_ptr_);
    collada_ptr_ = NULL;
        
    weak_ptr_.reset();
  }
    
  /* End of declaration of public function members */
    
} /* namespace graphics */
