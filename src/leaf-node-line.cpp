//
//  leaf-node-line.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-line.h>

#include <osg/Point>
#include <osg/CullFace>
#include <osg/LineWidth>
#include <gepetto/viewer/node.h>

namespace gepetto {
namespace viewer {
    int getNodeMode (LeafNodeLine* node) { return node->getMode(); }
    void setNodeMode (LeafNodeLine* node, const int& v) { node->setMode((GLenum)v); }

    void setNodePointSize (LeafNodeLine* node, const float& size)
    {
      ::osg::GeometryRefPtr geom = node->geometry ();
      osg::Point* point = static_cast<osg::Point*>(
          geom->getOrCreateStateSet()->getAttribute(osg::StateAttribute::POINT));
      point->setSize(size);
      geom->dirtyDisplayList();
    }

    void setNodeLineWidth (LeafNodeLine* node, const float& width)
    {
      ::osg::GeometryRefPtr geom = node->geometry ();
      osg::LineWidth* linewidth = static_cast<osg::LineWidth*>(
        geom->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINEWIDTH));
      linewidth->setWidth(width);
      geom->dirtyDisplayList();
    }

    
    /* Declaration of private function members */
    void LeafNodeLine::init ()
    {
        /* Init the beam as a Geometry */
        beam_ptr_ = new ::osg::Geometry();
        
        /* Define points of the beam */
        points_ptr_ = new ::osg::Vec3Array(2);
        
        /* Define the color */
        color_ptr_ = new ::osg::Vec4Array(1);
        
        beam_ptr_->setVertexArray(points_ptr_.get());
        beam_ptr_->setColorArray(color_ptr_.get(), ::osg::Array::BIND_PER_PRIMITIVE_SET);
        drawArray_ptr_ = new osg::DrawArrays(GL_LINE_STRIP,0,2);
        beam_ptr_->addPrimitiveSet(drawArray_ptr_.get());
        
        /* Create Geode for adding ShapeDrawable */
        geode_ptr_ = new osg::Geode();
        geode_ptr_->addDrawable(beam_ptr_);
        
        /* Add geode to the queue */
        this->asQueue()->addChild(geode_ptr_);
        
        /* Allow transparency */
        geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);
        
        /* No light influence by default */
        setLightingMode(LIGHT_INFLUENCE_OFF);
        
        /* Set a default line width */
        osg::LineWidth* linewidth = new osg::LineWidth();
        linewidth->setWidth(1.0f);
        beam_ptr_->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);

        osg::Point* point = new osg::Point(3.f);
        beam_ptr_->getOrCreateStateSet()->setAttribute(point, osg::StateAttribute::ON);

        addProperty(FloatProperty::create("PointSize",
              FloatProperty::getterFromMemberFunction(point, &osg::Point::getSize),
              FloatProperty::Setter_t(boost::bind(setNodePointSize, this, _1))));
        addProperty(FloatProperty::create("LineWidth",
              FloatProperty::getterFromMemberFunction(linewidth, &osg::LineWidth::getWidth),
              FloatProperty::Setter_t(boost::bind(setNodeLineWidth, this, _1))));
        addProperty(EnumProperty::create("ImmediateMode", glImmediateModeEnum(),
              EnumProperty::Getter_t(boost::bind(getNodeMode, this)),
              EnumProperty::Setter_t(boost::bind(setNodeMode, this, _1))));
    }
    
    LeafNodeLine::LeafNodeLine (const std::string& name, const osgVector3& start_point, const osgVector3& end_point) :
        NodeDrawable (name)
    {
        init ();
        setStartPoint(start_point);
        setEndPoint(end_point);
        setColor(osgVector4(1.,1.,1.,1.));
    }

    LeafNodeLine::LeafNodeLine (const std::string& name, const osgVector3& start_point, const osgVector3& end_point, const osgVector4& color) :
        NodeDrawable (name)
    {
        init ();
        setStartPoint(start_point);
        setEndPoint(end_point);
        setColor(color);
    }

    LeafNodeLine::LeafNodeLine (const std::string& name, const ::osg::Vec3ArrayRefPtr& points, const osgVector4& color) :
        NodeDrawable (name)
    {
        init ();
        setPoints(points);
        setColor(color);
    }

    LeafNodeLine::LeafNodeLine (const LeafNodeLine& other) :
        NodeDrawable (other)
    {
        init();
        setPoints (other.points_ptr_);
        setColor(other.getColor());
    }
    
    void LeafNodeLine::initWeakPtr (LeafNodeLineWeakPtr other_weak_ptr )
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */
    LeafNodeLinePtr_t LeafNodeLine::create (const std::string& name, const osgVector3& start_point, const osgVector3& end_point)
    {
        LeafNodeLinePtr_t shared_ptr(new LeafNodeLine (name, start_point, end_point));
        
        // Add reference to itself
        shared_ptr->initWeakPtr (shared_ptr);
        
        return shared_ptr;
    }
    
    LeafNodeLinePtr_t LeafNodeLine::create (const std::string& name, const osgVector3& start_point, const osgVector3& end_point, const osgVector4& color)
    {
        LeafNodeLinePtr_t shared_ptr(new LeafNodeLine (name, start_point, end_point, color));

        // Add reference to itself
        shared_ptr->initWeakPtr (shared_ptr);

        return shared_ptr;
    }
    
    LeafNodeLinePtr_t LeafNodeLine::create (const std::string& name, const ::osg::Vec3ArrayRefPtr& points, const osgVector4& color)
    {
        LeafNodeLinePtr_t shared_ptr(new LeafNodeLine (name, points, color));

        // Add reference to itself
        shared_ptr->initWeakPtr (shared_ptr);

        return shared_ptr;
    }

    LeafNodeLinePtr_t LeafNodeLine::createCopy (LeafNodeLinePtr_t other)
    {
        LeafNodeLinePtr_t shared_ptr(new LeafNodeLine (*other));
        
        // Add reference to itself
        shared_ptr->initWeakPtr (shared_ptr);
        
        return shared_ptr;
    }
    
    /* End of declaration of protected function members */
    
    /* Declaration of public function members */
    
    LeafNodeLinePtr_t LeafNodeLine::clone (void) const
    {
        return LeafNodeLine::createCopy(weak_ptr_.lock());
    }
    
    LeafNodeLinePtr_t LeafNodeLine::self (void) const
    {
        return weak_ptr_.lock();
    }
    
    void LeafNodeLine::setStartPoint (const osgVector3& start_point)
    {
        points_ptr_->at(0) = start_point;
        beam_ptr_->dirtyDisplayList();
        setDirty();
    }
    
    osgVector3 LeafNodeLine::getStartPoint() const
    {
        return points_ptr_->at(0);
    }

    void LeafNodeLine::setEndPoint (const osgVector3& end_point)
    {
        points_ptr_->at(1) = end_point;
        beam_ptr_->dirtyDisplayList();
        setDirty();
    }
    
    osgVector3 LeafNodeLine::getEndPoint() const
    {
        return points_ptr_->at(1);
    }
    
    void LeafNodeLine::setMode (const GLenum& mode)
    {
      drawArray_ptr_->set (mode, 0, (GLsizei)points_ptr_->size ());
      beam_ptr_->dirtyDisplayList();
      setDirty();
    }

    GLenum LeafNodeLine::getMode () const
    {
      return drawArray_ptr_->getMode();
    }

    void LeafNodeLine::setPoints (const osgVector3& start_point, const osgVector3& end_point)
    {        
        setStartPoint(start_point);
        setEndPoint(end_point);
    }

    void LeafNodeLine::setPoints (const ::osg::Vec3ArrayRefPtr& points)
    {
        bool sizeChanged = (points->size() != points_ptr_->size());
        points_ptr_ = points;
        beam_ptr_->setVertexArray (points_ptr_.get ());
        if (sizeChanged)
          setPointsSubset (0, points->size());
        else {
          beam_ptr_->dirtyDisplayList();
          setDirty();
        }
    }

    void LeafNodeLine::setPointsSubset (const int first, const std::size_t count)
    {
        if (first + count > points_ptr_->size())
          throw std::invalid_argument ("Invalid range of points in LeafNodeLine::setPointsSubset");
        drawArray_ptr_->setFirst (first);
        drawArray_ptr_->setCount ((GLsizei)count);
        beam_ptr_->dirtyDisplayList();
        setDirty();
    }

    void LeafNodeLine::setColor (const osgVector4& color)
    {      
        color_ptr_->at(0) = color;
        beam_ptr_->dirtyDisplayList();
        setTransparentRenderingBin (color[3] < Node::TransparencyRenderingBinThreshold);
        setDirty();
    }
  
    void LeafNodeLine::setColors (const ::osg::Vec4ArrayRefPtr & colors)
    {
      color_ptr_ = colors;
      beam_ptr_->dirtyDisplayList();
      bool transparent = false;
      for (std::size_t i = 0; i < colors->size(); ++i) {
        if (colors->at(i)[3] < Node::TransparencyRenderingBinThreshold) {
          transparent = true;
          break;
        }
      }
      setTransparentRenderingBin (transparent);
      setDirty();
    }

    // void LeafNodeLine::setLineStipple (const GLint factor, const GLushort pattern)
    // {
      // osg::LineStipple* stipple = static_cast<osg::LineStipple*>(
        // beam_ptr_->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINESTIPPLE));
      // stipple->setFactor(factor);
      // stipple->setPattern(pattern);
      // beam_ptr_->dirtyDisplayList();
    // }

    LeafNodeLine::~LeafNodeLine()
    {
        /* Proper deletion of all tree scene */
        geode_ptr_->removeDrawable(beam_ptr_);
        //std::cout << "Beam ref count " << beam_ptr_->referenceCount() << std::endl;
        beam_ptr_ = NULL;
        
        this->asQueue()->removeChild(geode_ptr_);
        //std::cout << "Geode ref count " << geode_ptr_->referenceCount() << std::endl;
        geode_ptr_ = NULL;
        
        weak_ptr_.reset();
        //std::cout << "Destruction of line " << getID() << std::endl;
    }
    
    /* End of declaration of public function members */
    
} /* namespace viewer */

} /* namespace gepetto */
