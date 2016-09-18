//
//  leaf-node-mesh.cpp
//  gepetto-viewer
//
//  Created by Olivier Stasse, Anthony Couret, Mathieu Geisert.
//  Copyright (c) 2016 LAAS-CNRS. All rights reserved.
//

#include <sys/stat.h>
#include <fstream>
#include <ios>
#include <gepetto/viewer/leaf-node-mesh.h>
#include <osgDB/ReadFile>

namespace graphics {
    
  /* Declaration of private function members */

  void LeafNodeMesh::init()
  {
    
    /* Create Geode for adding ShapeDrawable */
    geode_ptr_ = new osg::Geode ();

    /* Create mesh geometry */
    mesh_geometry_ptr_ = new osg::Geometry();
    geode_ptr_->addDrawable(mesh_geometry_ptr_);
    
    /* Create PositionAttitudeTransform */
    this->asQueue()->addChild(geode_ptr_);
        
    /* Allow transparency */
    if (mesh_geometry_ptr_->getOrCreateStateSet())
      {
	osg::ref_ptr<osg::StateSet> nodess (mesh_geometry_ptr_->getOrCreateStateSet());
	nodess->setMode(GL_BLEND, ::osg::StateAttribute::OFF);
	// Create Material and assign color.

	// Creating the material object
	osg::ref_ptr<osg::Material> mat (new osg::Material);

	//Attaching the newly defined state set object to the node state set
	mat->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
	//mat->setColorMode(osg::Material::SPECULAR);
	nodess->setAttribute(mat.get());

      }
  }
    
  LeafNodeMesh::LeafNodeMesh(const std::string & name) :
    Node(name)
  {
    init();
  }
    
  LeafNodeMesh::LeafNodeMesh(const std::string &name, 
			     const osgVector4& color_diffuse) :
    Node(name)
  {
    init();
    setColor(color_diffuse);
  }
  
  LeafNodeMesh::LeafNodeMesh(const graphics::LeafNodeMesh& other) :
    Node(other.getID())
  {
    init();
  }
    

  void LeafNodeMesh::initWeakPtr(LeafNodeMeshWeakPtr other_weak_ptr)
  {
    weak_ptr_ = other_weak_ptr;
  }
    
  /* End of declaration of private function members */
    
  /* Declaration of protected function members */
    
  LeafNodeMeshPtr_t LeafNodeMesh::create(const std::string& name)
  {
    LeafNodeMeshPtr_t shared_ptr(new LeafNodeMesh
                                    (name));
    
    // Add reference to itself
    shared_ptr->initWeakPtr(shared_ptr);
    
    return shared_ptr;
  }

  LeafNodeMeshPtr_t LeafNodeMesh::create(const std::string& name, const osgVector4& color)
  {
    LeafNodeMeshPtr_t shared_ptr(new LeafNodeMesh
                                    (name, color));
    
    // Add reference to itself
    shared_ptr->initWeakPtr(shared_ptr);
    
    return shared_ptr;
  }
    
  LeafNodeMeshPtr_t LeafNodeMesh::createCopy(LeafNodeMeshPtr_t other)
  {
    LeafNodeMeshPtr_t shared_ptr(new LeafNodeMesh(*other));
        
    // Add reference to itself
    shared_ptr->initWeakPtr(shared_ptr);
        
    return shared_ptr;
  }


  /* End of declaration of protected function members */
    
  /* Declaration of public function members */
    
  LeafNodeMeshPtr_t LeafNodeMesh::clone(void) const
  {
    return LeafNodeMesh::createCopy(weak_ptr_.lock());
  }
    
  LeafNodeMeshPtr_t LeafNodeMesh::self(void) const
  {
    return weak_ptr_.lock();
  }

    
  void LeafNodeMesh::setColor(const osgVector4& color_diffuse,
			      const osgVector4& color_specular,
			      const osgVector4& color_emissive)
  {
    //setColor(collada_ptr_,color);
    osg::ref_ptr<osg::Material> mat_ptr (new osg::Material); 
    osgVector4 color_zero(0.0f,0.0f,0.0f,0.0f);
    mat_ptr->setDiffuse(osg::Material::FRONT_AND_BACK,color_diffuse); 
    mat_ptr->setAmbient(osg::Material::FRONT_AND_BACK,color_zero); 
    mat_ptr->setSpecular(osg::Material::FRONT_AND_BACK,color_specular); 
    mat_ptr->setEmission(osg::Material::FRONT_AND_BACK,color_emissive); 

    if (mesh_geometry_ptr_->getStateSet())
      mesh_geometry_ptr_->getStateSet()->setAttribute(mat_ptr.get());    
  }
  void LeafNodeMesh::setColor(const osgVector4& color_diffuse)
  {
    osgVector4 color_specular(0.0f,0.0f,0.0f,0.0f),
      color_emissive(0.0f,0.0f,0.0f,0.0f);
    setColor(color_diffuse,color_specular,color_emissive);

  }

  void LeafNodeMesh::setAlpha(const float& alpha)
  {
    osg::StateSet* ss = mesh_geometry_ptr_->getStateSet();
    if (ss)
      {
	alpha_ = alpha;
	osg::Material *mat;
	if (ss->getAttribute(osg::StateAttribute::MATERIAL))
	  mat = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
	else
	  {
	    mat = new osg::Material;
	    ss->setAttribute(mat, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	  }
	mat->setTransparency(osg::Material::FRONT_AND_BACK, alpha);
	if (alpha == 0)
	  ss->setRenderingHint(osg::StateSet::DEFAULT_BIN);
	else
	  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
      }
  }
 
  void LeafNodeMesh::setTexture(const std::string& image_path)
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
    mesh_geometry_ptr_->getStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
  }

  osg::ref_ptr<osg::Node> LeafNodeMesh::getOsgNode() const
  {
    return geode_ptr_;
  }

  void LeafNodeMesh::setVertexArray(osg::Vec3ArrayRefPtr arrayOfVertices)
  {
    mesh_geometry_ptr_->setVertexArray(arrayOfVertices);
  }

  void LeafNodeMesh::addPrimitiveSet(osg::DrawElementsUInt * aPrimitiveSet)
  {
    mesh_geometry_ptr_->addPrimitiveSet(aPrimitiveSet);
  }

  void LeafNodeMesh::setColorArray(osg::Vec4ArrayRefPtr aSetOfColors)
  {
    mesh_geometry_ptr_->setColorArray(aSetOfColors);
  }
  
  void LeafNodeMesh::setColorBinding(osg::Geometry::AttributeBinding aColorBinding)
  {
    mesh_geometry_ptr_->setColorBinding(aColorBinding);
  }

  void LeafNodeMesh::setNormalArray(osg::Vec3ArrayRefPtr normals)
  {
    mesh_geometry_ptr_->setNormalArray(normals);
  }

  void LeafNodeMesh::setNormalBinding(osg::Geometry::AttributeBinding aNormalBinding)
  {
    mesh_geometry_ptr_->setNormalBinding(aNormalBinding);
  }

  LeafNodeMesh::~LeafNodeMesh()
  {
    /* Proper deletion of all tree scene */
    this->asQueue()->removeChild(geode_ptr_);
    mesh_geometry_ptr_ = NULL;
        
    weak_ptr_.reset();
  }
    
  /* End of declaration of public function members */
    
} /* namespace graphics */
