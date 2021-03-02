//
//  leaf-node-ground.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-ground.h>

namespace gepetto {
namespace viewer {
    
    /* Declaration of private function members */
    void LeafNodeGround::init()
    {
      for (auto* n : {&nX_, &nY_}) {
        n->min = 2;
        n->step = 2;
        n->callback(boost::bind(&LeafNodeGround::updateVertices, this));
      }
      for (auto* size : {&square_size_x_, &square_size_y_}) {
        size->min = 0.000001f;
        size->step = 0.1f;
        size->adaptiveDecimal = true;
        size->callback(boost::bind(&LeafNodeGround::updateVertices, this));
      }

      /* Allocation of vertices */
      vertices_array_ptr_ = new ::osg::Vec3Array;
      colors_array_ptr_ = new ::osg::Vec4Array;
      
      /* Allocating grid_geometry_ptr_ */
      if (!grid_geometry_ptr_.valid()) {
        grid_geometry_ptr_ = new ::osg::Geometry;
      }
      
      grid_geometry_ptr_->setVertexArray(vertices_array_ptr_);
      grid_geometry_ptr_->setColorArray(colors_array_ptr_);
      
      grid_geometry_ptr_->setColorBinding(::osg::Geometry::BIND_PER_VERTEX);
      
      /* Define the normal to all quads */
      ::osg::Vec3ArrayRefPtr normals_array_ptr = new ::osg::Vec3Array;
      normals_array_ptr->push_back( osgVector3(0.0f,0.0f, 1.0f) );
      grid_geometry_ptr_->setNormalArray( normals_array_ptr );
      grid_geometry_ptr_->setNormalBinding( ::osg::Geometry::BIND_OVERALL );
      
      /* Defining type of geometries */
      draw_array_ptr_ = new osg::DrawArrays (::osg::PrimitiveSet::QUADS, 0, 0);
      grid_geometry_ptr_->addPrimitiveSet (draw_array_ptr_);

      updateVertices();
      
      
        /* Allocating geode_ptr_ */
        if (!geode_ptr_.valid()) {
            geode_ptr_ = new ::osg::Geode;
        }
        
        geode_ptr_->addDrawable(grid_geometry_ptr_);
      
        //node_osg_ptr_->asGroup()->addChild(geode_ptr_);
        asQueue()->addChild(geode_ptr_);
        
        /* Apply colors */
        setColors(color1_,color2_);

        if (hasProperty("Color"))
          properties_.erase("Color");
        addProperty(Vector4Property::create("Color1",
              Vector4Property::getterFromMemberFunction(this, &LeafNodeGround::getColor1),
              Vector4Property::setterFromMemberFunction(this, &LeafNodeGround::setColor1)));
        addProperty(Vector4Property::create("Color2",
              Vector4Property::getterFromMemberFunction(this, &LeafNodeGround::getColor2),
              Vector4Property::setterFromMemberFunction(this, &LeafNodeGround::setColor2)));
        addProperty(&nX_);
        addProperty(&nY_);
        addProperty(&square_size_x_);
        addProperty(&square_size_y_);
    }

    void LeafNodeGround::updateVertices()
    {
      float sx = square_size_x_.value;
      float sy = square_size_y_.value;
      int nx = nX_.value;
      int ny = nY_.value;

      osgVector3 corner = osgVector3( - (float)nx * sx / 2 , - (float)ny * sy / 2 , 0.f );
      
      /* Allocation of vertices */
      vertices_array_ptr_->resize(nx*ny*4);
      colors_array_ptr_->resize(nx*ny*4);

      int k = 0;
      for ( int j(0) ; j < ny ; j++ )
      {
        for ( int i(0) ; i < nx ; i++ )
        {
          vertices_array_ptr_->at(k+0) = corner + osgVector3(sx * (float) i , sy * float(j+1), 0.f);
          vertices_array_ptr_->at(k+1) = corner + osgVector3(sx * (float) i , sy * (float) j , 0.f);
          vertices_array_ptr_->at(k+2) = corner + osgVector3(sx * float(i+1), sy * (float) j , 0.f);
          vertices_array_ptr_->at(k+3) = corner + osgVector3(sx * float(i+1), sy * float(j+1), 0.f);
          
          if ((i+j)%2) {
            colors_array_ptr_->at(k+0) = color1_;
            colors_array_ptr_->at(k+1) = color1_;
            colors_array_ptr_->at(k+2) = color1_;
            colors_array_ptr_->at(k+3) = color1_;
          }
          else {
            colors_array_ptr_->at(k+0) = color2_;
            colors_array_ptr_->at(k+1) = color2_;
            colors_array_ptr_->at(k+2) = color2_;
            colors_array_ptr_->at(k+3) = color2_;
          }
          k += 4;
        }
      }
      
      grid_geometry_ptr_->setColorBinding(::osg::Geometry::BIND_PER_VERTEX);
      
      /* Defining type of geometries */
      draw_array_ptr_->setCount( (GLsizei) vertices_array_ptr_->size ());
      grid_geometry_ptr_->dirtyDisplayList();
      setDirty();

    }
    
    void LeafNodeGround::initWeakPtr( const LeafNodeGroundWeakPtr &other_weak_ptr )
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */
    
    
    LeafNodeGround::LeafNodeGround(const std::string& name, int nX, int nY,
        float square_length, float square_width,
        const osgVector4& color1, const osgVector4& color2)
      : Node(name),
      square_size_x_ ("CellSizeX"),
      square_size_y_ ("CellSizeY"),
      nX_ ("NbCellsX"),
      nY_ ("NbCellsY"),
      color1_(color1), color2_(color2)
    {
      nX_.value = nX;
      nY_.value = nY;
      square_size_x_.value = square_length;
      square_size_y_.value = square_width;
      init();
    }
    
    /* End of declaration of protected function members */
  
    /* Declaration of public function members */
  
    LeafNodeGroundPtr_t LeafNodeGround::create(const std::string& name,
        int nX, int nY, float square_length, float square_width)
    {
      LeafNodeGroundPtr_t shared_ptr( new LeafNodeGround(name, nX, nY,
            square_length, square_width, osgVector4(0.,0.,0.,1.), osgVector4(1.,1.,1.,1.)) );
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }

    LeafNodeGroundPtr_t LeafNodeGround::create(const std::string& name)
    {
      return create(name, 10, 10, 1.f, 1.f);
    }
    
    LeafNodeGroundPtr_t LeafNodeGround::self(void) const
    {
        return weak_ptr_.lock();
    }
  
    void LeafNodeGround::setColor(const osgVector4 &color)
    {
        setColor1(color);
    }
  
    void LeafNodeGround::setColor1(const osgVector4 &color1)
    {
        LeafNodeGround::setColors(color1, color2_);
    }
    
    void LeafNodeGround::setColor2(const osgVector4 &color2)
    {
        LeafNodeGround::setColors(color1_, color2);
    }
    
    void LeafNodeGround::setColors(const osgVector4 &color1 , const osgVector4 &color2)
    {
      color1_ = color1;
      color2_ = color2;

      /* Set colors */
      int k = 0;
      for ( int j(0) ; j < nY_.value ; j++ ) {
        for ( int i(0) ; i < nX_.value ; i++ ) {
          if ((i+j)%2) {
            colors_array_ptr_->at(k+0) = color1_;
            colors_array_ptr_->at(k+1) = color1_;
            colors_array_ptr_->at(k+2) = color1_;
            colors_array_ptr_->at(k+3) = color1_;
          } else {
            colors_array_ptr_->at(k+0) = color2_;
            colors_array_ptr_->at(k+1) = color2_;
            colors_array_ptr_->at(k+2) = color2_;
            colors_array_ptr_->at(k+3) = color2_;
          }
          k += 4;
        }
      }

      /* Apply colors */
      setTransparentRenderingBin (   color1[3] < Node::TransparencyRenderingBinThreshold
          || color2[3] < Node::TransparencyRenderingBinThreshold);
      setDirty();
    }

    LeafNodeGround::~LeafNodeGround()
    {
      
      /* Proper deletion of all tree scene */
      geode_ptr_->removeDrawable(grid_geometry_ptr_);
      grid_geometry_ptr_ = NULL;
        
      this->asQueue()->removeChild(geode_ptr_);
      geode_ptr_ = NULL;

      colors_array_ptr_.release();

      weak_ptr_.reset();
      
    }
  
    /* End of declaration of public function members */
} /* namespace viewer */

}
