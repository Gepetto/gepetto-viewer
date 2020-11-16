//
//  leaf-node-point-cloud.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier
//  Copyright (c) 2020 Inria. All rights reserved.
//

#include <gepetto/viewer/leaf-node-point-cloud.h>

namespace gepetto {
namespace viewer {
    
    /* Declaration of private function members */
    
    void LeafNodePointCloud::init()
    {
      /* Allocating geometry_ptr_ */
      if (!geometry_ptr_.valid()) {
        geometry_ptr_ = new ::osg::Geometry;
      }
      
      geometry_ptr_->setVertexArray(points_array_ptr_);
      geometry_ptr_->setColorArray(colors_array_ptr_);
      geometry_ptr_->setColorBinding(::osg::Geometry::BIND_PER_VERTEX);
      
      /* Defining type of geometries */
      geometry_ptr_->addPrimitiveSet
      (new ::osg::DrawArrays(::osg::PrimitiveSet::POINTS, 0,
                             (GLsizei) points_array_ptr_->size ()));
      

      /* Allocating geode_ptr_ */
      if (!geode_ptr_.valid())
        geode_ptr_ = new ::osg::Geode;

      geode_ptr_->addDrawable(geometry_ptr_);
      osg::StateSet* state = geometry_ptr_->getOrCreateStateSet();
      state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      
      //node_osg_ptr_->asGroup()->addChild(geode_ptr_);
      asQueue()->addChild(geode_ptr_);
    }
    
    void LeafNodePointCloud::initWeakPtr( const LeafNodePointCloudWeakPtr &other_weak_ptr )
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */

    LeafNodePointCloud::LeafNodePointCloud(const std::string & name,
                                           const ::osg::Vec3ArrayRefPtr points,
                                           const ::osg::Vec4ArrayRefPtr colors)
    : Node(name)
    , points_array_ptr_(points)
    , colors_array_ptr_(colors)
    {
      init();
    }
    
    LeafNodePointCloud::LeafNodePointCloud(const LeafNodePointCloud & other)
    : Node(other.getID())
    {
      *points_array_ptr_.get() = *other.points_array_ptr_.get();
      *colors_array_ptr_.get() = *other.colors_array_ptr_.get();
      init();
    }
  
    /* End of declaration of protected function members */
  
    /* Declaration of public function members */
  
    LeafNodePointCloudPtr_t LeafNodePointCloud::create(const std::string & name,
                                                       const ::osg::Vec3ArrayRefPtr points,
                                                       const ::osg::Vec4ArrayRefPtr colors)
    {
      LeafNodePointCloudPtr_t shared_ptr(new LeafNodePointCloud(name, points, colors));
      
      // Add reference to itself
      shared_ptr->initWeakPtr(shared_ptr);
      
      return shared_ptr;
    }

    LeafNodePointCloudPtr_t LeafNodePointCloud::create(const std::string & name,
                                                       const ::osg::Vec3ArrayRefPtr points,
                                                       const osgVector4 & color)
    {
      ::osg::Vec4ArrayRefPtr colors = ::osg::Vec4ArrayRefPtr(new ::osg::Vec4Array((unsigned int)points->size()));
      for(size_t k = 0; k < points->size(); ++k)
        (*colors)[k] = color;
      return create(name,points,colors);
    }

    LeafNodePointCloudPtr_t LeafNodePointCloud::createCopy(const LeafNodePointCloudPtr_t &other)
    {
      LeafNodePointCloudPtr_t shared_ptr( new LeafNodePointCloud( *other ) );
        
      // Add reference to itself
      shared_ptr->initWeakPtr(shared_ptr);
      
      return shared_ptr;
    }
    
    LeafNodePointCloudPtr_t LeafNodePointCloud::clone(void) const
    {
        return LeafNodePointCloud::createCopy( weak_ptr_.lock() );
    }
    
    LeafNodePointCloudPtr_t LeafNodePointCloud::self(void) const
    {
        return weak_ptr_.lock();
    }

    
    void LeafNodePointCloud::setColor(const osgVector4 & color)
    {
      ::osg::Vec4ArrayRefPtr colors_array = ::osg::Vec4ArrayRefPtr(new ::osg::Vec4Array((unsigned int)points_array_ptr_->size()));
      for(size_t k = 0; k < points_array_ptr_->size(); ++k)
        (*colors_array)[k] = color;
      
      setColors(colors_array);
    }
    
    void LeafNodePointCloud::setColors(const ::osg::Vec4ArrayRefPtr colors_array)
    {
        
      if(geometry_ptr_)
      {
        if(colors_array->size() != points_array_ptr_->size())
        {
          std::stringstream ss;
          ss << "Got size: " << colors_array->size() << ". Expected: " << points_array_ptr_->size() << std::endl;
          throw std::invalid_argument("The dimension of the array of colors does not match the number of points in the cloud." + ss.str());
        }
      }
      
      if(colors_array_ptr_.get() != colors_array.get())
      {
        colors_array_ptr_.release();
        *colors_array_ptr_.get() = *colors_array.get();
      }
      
      bool transparent_hint = false;
      for(size_t k = 0; k < colors_array_ptr_->getElementSize(); ++k)
      {
        if((*colors_array_ptr_)[k][3] < Node::TransparencyRenderingBinThreshold)
        {
          transparent_hint = true; break;
        }
      }
      
      /* Apply colors */
      geometry_ptr_->setColorArray(colors_array_ptr_.get());
      geometry_ptr_->setColorBinding(::osg::Geometry::BIND_PER_VERTEX);
      setTransparentRenderingBin(transparent_hint);
      setDirty();
    }

    void LeafNodePointCloud::setPoints(const ::osg::Vec3ArrayRefPtr points_array,
                                       const ::osg::Vec4ArrayRefPtr colors_array)
    {
      if(geometry_ptr_)
      {
        if(points_array->size() != colors_array->size())
        {
          std::stringstream ss;
          ss << "Got point cloud size: " << points_array->size() << " and color array size:" << colors_array->size() << std::endl;
          throw std::invalid_argument("The dimension of the array of colors does not match the number of points in the cloud." + ss.str());
        }
      }
      
      if(points_array_ptr_.get() != points_array.get())
      {
        points_array_ptr_.release();
        *points_array_ptr_.get() = *points_array.get();
      }
      
      /* Change points */
      geometry_ptr_->setVertexArray(points_array_ptr_.get());
      setColors(colors_array);
      setDirty();
    }
  
    LeafNodePointCloud::~LeafNodePointCloud()
    {
      
      /* Proper deletion of all tree scene */
      geode_ptr_->removeDrawable(geometry_ptr_);
      geometry_ptr_ = NULL;
        
      this->asQueue()->removeChild(geode_ptr_);
      geode_ptr_ = NULL;

      colors_array_ptr_.release();
      points_array_ptr_.release();

      weak_ptr_.reset();
    }
  
    /* End of declaration of public function members */
} /* namespace viewer */

}
