//
//  leaf-node-ground.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-ground.h>

namespace graphics {
    
    /* Declaration of private function members */
    
    void LeafNodeGround::init()
    {
      //node_osg_ptr_ = ::graphics::NodeOSG::create(getCenter(), getRotation(), getConfiguration());
      osgVector3 center = osgVector3( length_ , width_ , 0.f );
      
      /* Number of cells on both dimensions */
      float nX = floorf( 2.f * length_ / square_length_ );
      float nY = floorf( 2.f * width_ / square_width_ );
      
      /* According to floor operation, we adapt sizes */
      //setSquareLength(2. * length_ / nX);
      //setSquareWidth(2. * width_ / nY);
      
      /* Setting base vectors */
      osgVector3 x_base_square = osgVector3( square_length_ , 0.0f , 0.0f );
      osgVector3 y_base_square = osgVector3( 0.0f , square_width_ , 0.0f );
      
      /* Allocation of vertices */
      ::osg::Vec3ArrayRefPtr vertices_array_ptr = new ::osg::Vec3Array;
      colors_array_ptr_ = new ::osg::Vec4Array;
      
      for ( int j(0) ; j < nY ; j++ )
      {
        for ( int i(0) ; i < nX ; i++ )
        {
          vertices_array_ptr->push_back( - center + x_base_square * ((float) i) + y_base_square * ((float) (j+1)));
          vertices_array_ptr->push_back( - center + x_base_square * ((float) i) + y_base_square * ((float) j));
          vertices_array_ptr->push_back( - center + x_base_square * ((float) (i+1)) + y_base_square * ((float) j));
          vertices_array_ptr->push_back( - center + x_base_square * ((float) (i+1)) + y_base_square * ((float) (j+1)));
          
          if ((i+j)%2) {
            colors_array_ptr_->push_back(color1_);
            colors_array_ptr_->push_back(color1_);
            colors_array_ptr_->push_back(color1_);
            colors_array_ptr_->push_back(color1_);
          }
          else {
            colors_array_ptr_->push_back(color2_);
            colors_array_ptr_->push_back(color2_);
            colors_array_ptr_->push_back(color2_);
            colors_array_ptr_->push_back(color2_);
          }
        }
      }
      
      /* Allocating grid_geometry_ptr_ */
      if (~grid_geometry_ptr_.valid()) {
        grid_geometry_ptr_ = new ::osg::Geometry;
      }
      
      grid_geometry_ptr_->setVertexArray(vertices_array_ptr);
      grid_geometry_ptr_->setColorArray(colors_array_ptr_);
      
      grid_geometry_ptr_->setColorBinding(::osg::Geometry::BIND_PER_VERTEX);
      
      /* Define the normal to all quads */
      ::osg::Vec3ArrayRefPtr normals_array_ptr = new ::osg::Vec3Array;
      normals_array_ptr->push_back( osgVector3(0.0f,0.0f, 1.0f) );
      grid_geometry_ptr_->setNormalArray( normals_array_ptr );
      grid_geometry_ptr_->setNormalBinding( ::osg::Geometry::BIND_OVERALL );
      
      /* Defining type of geometries */
      grid_geometry_ptr_->addPrimitiveSet
	(new ::osg::DrawArrays (::osg::PrimitiveSet::QUADS, 0,
				(GLsizei) vertices_array_ptr->size ()));
      
      
        /* Allocating geode_ptr_ */
        if (~geode_ptr_.valid()) {
            geode_ptr_ = new ::osg::Geode;
        }
        
        geode_ptr_->addDrawable(grid_geometry_ptr_);
      
        //node_osg_ptr_->asGroup()->addChild(geode_ptr_);
        asQueue()->addChild(geode_ptr_);
        
        /* Apply colors */
        setColors(color1_,color2_);

        addProperty(Vector4Property::create("Color1",
              Vector4Property::getterFromMemberFunction(this, &LeafNodeGround::getColor1),
              Vector4Property::setterFromMemberFunction(this, &LeafNodeGround::setColor1)));
        addProperty(Vector4Property::create("Color2",
              Vector4Property::getterFromMemberFunction(this, &LeafNodeGround::getColor2),
              Vector4Property::setterFromMemberFunction(this, &LeafNodeGround::setColor2)));
        
#ifdef DEBUG
        std::cout << getID() << " created" << std::endl;
#endif
    }
    
    void LeafNodeGround::initWeakPtr( const LeafNodeGroundWeakPtr &other_weak_ptr )
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */
    
    
    LeafNodeGround::LeafNodeGround(const std::string& name, const float &length, const float &width, const float &square_length, const float &square_width, const osgVector4& color1, const osgVector4& color2):
        Node(name), length_(length), width_(width), square_length_(square_length), square_width_(square_width), color1_(color1), color2_(color2)
        {
            init();
        }
    
    LeafNodeGround::LeafNodeGround(const LeafNodeGround &other) :
        Node(other.getID()), length_(other.length_), width_(other.width_), square_length_(other.square_length_), square_width_(other.square_width_), color1_(other.getColor1()), color2_(other.getColor2())
        {
            init();
        }
  
    /* End of declaration of protected function members */
  
    /* Declaration of public function members */
  
    LeafNodeGroundPtr_t LeafNodeGround::create(const std::string& name, const float& length, const float& width)
    {

        LeafNodeGroundPtr_t shared_ptr( new LeafNodeGround(name, length, width, length/10.f, width/10.f, osgVector4(0.,0.,0.,1.), osgVector4(1.,1.,1.,1.)) );
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }
    
    LeafNodeGroundPtr_t LeafNodeGround::create(const std::string& name, const float &length, const float &width, const float &square_length, const float &square_width)
    {
      LeafNodeGroundPtr_t shared_ptr( new LeafNodeGround(name, length, width, square_length, square_width, osgVector4(0.,0.,0.,1.), osgVector4(1.,1.,1.,1.)) );
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }

    LeafNodeGroundPtr_t LeafNodeGround::create(const std::string& name)
    {
      LeafNodeGroundPtr_t shared_ptr( new LeafNodeGround(name, 10.f, 10.f, 1.f, 1.f, osgVector4(0.,0.,0.,1.), osgVector4(1.,1.,1.,1.)) );

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }
    
    LeafNodeGroundPtr_t LeafNodeGround::createCopy(const LeafNodeGroundPtr_t &other)
    {
      LeafNodeGroundPtr_t shared_ptr( new LeafNodeGround( *other ) );
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }
    
    LeafNodeGroundPtr_t LeafNodeGround::clone(void) const
    {
        return LeafNodeGround::createCopy( weak_ptr_.lock() );
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

        /* Reset colors array */
        colors_array_ptr_.release();
        colors_array_ptr_ = new ::osg::Vec4Array;
        
        /* Number of cells on both dimensions */
        float nX = floorf( 2.f * length_ / square_length_ );
        float nY = floorf( 2.f * width_ / square_width_ );
        
        /* Set colors */
        for ( int j(0) ; j < nY ; j++ )
        {
          for ( int i(0) ; i < nX ; i++ )
          {
            if ((i+j)%2) {
              colors_array_ptr_->push_back(color1_);
              colors_array_ptr_->push_back(color1_);
              colors_array_ptr_->push_back(color1_);
              colors_array_ptr_->push_back(color1_);
            }
            else {
              colors_array_ptr_->push_back(color2_);
              colors_array_ptr_->push_back(color2_);
              colors_array_ptr_->push_back(color2_);
              colors_array_ptr_->push_back(color2_);
            }
          }
        }
      
        /* Apply colors */
        grid_geometry_ptr_->setColorArray(colors_array_ptr_);
        grid_geometry_ptr_->setColorBinding(::osg::Geometry::BIND_PER_VERTEX);
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
}
