//
//  leaf-node-line.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-line.h>

#include <osg/CullFace>
#include <osg/LineWidth>
#include <gepetto/viewer/node.h>

namespace graphics {
    
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
        beam_ptr_->setColorArray(color_ptr_.get());
        beam_ptr_->setColorBinding(::osg::Geometry::BIND_PER_PRIMITIVE_SET);
        drawArray_ptr_ = new osg::DrawArrays(GL_LINE_STRIP,0,2);
        beam_ptr_->addPrimitiveSet(drawArray_ptr_.get());
        
        /* Create Geode for adding ShapeDrawable */
        geode_ptr_ = new osg::Geode();
        geode_ptr_->addDrawable(beam_ptr_);
        
        /* Add geode to the queue */
        this->asQueue()->addChild(geode_ptr_);
        
        /* Allow transparency */
        geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);
        
        /* No light influence */
        beam_ptr_->getOrCreateStateSet()->setMode(GL_LIGHTING, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
        
        /* Set a default line width */
        osg::LineWidth* linewidth = new osg::LineWidth();
        linewidth->setWidth(1.0f);
        beam_ptr_->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
    }
    
    LeafNodeLine::LeafNodeLine (const std::string& name, const osgVector3& start_point, const osgVector3& end_point) :
        graphics::Node (name)
    {
        init ();
        setStartPoint(start_point);
        setEndPoint(end_point);
        setColor(osgVector4(1.,1.,1.,1.));
    }

    LeafNodeLine::LeafNodeLine (const std::string& name, const osgVector3& start_point, const osgVector3& end_point, const osgVector4& color) :
        graphics::Node (name)
    {
        init ();
        setStartPoint(start_point);
        setEndPoint(end_point);
        setColor(color);
    }

    LeafNodeLine::LeafNodeLine (const std::string& name, const ::osg::Vec3ArrayRefPtr& points, const osgVector4& color) :
        graphics::Node (name)
    {
        init ();
        setPoints(points);
        setColor(color);
    }

    LeafNodeLine::LeafNodeLine (const LeafNodeLine& other) :
        graphics::Node (other)
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
    }
    
    osgVector3 LeafNodeLine::getStartPoint() const
    {
        return points_ptr_->at(0);
    }

    void LeafNodeLine::setEndPoint (const osgVector3& end_point)
    {
        points_ptr_->at(1) = end_point;
    }
    
    osgVector3 LeafNodeLine::getEndPoint() const
    {
        return points_ptr_->at(1);
    }
    
    void LeafNodeLine::setMode (const GLenum mode)
    {
      drawArray_ptr_->set (mode, 0, points_ptr_->size ());
      beam_ptr_->dirtyDisplayList();
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
        points_ptr_ = points;
        beam_ptr_->setVertexArray (points_ptr_.get ());
        drawArray_ptr_->setCount (points->size());
    }
    
    void LeafNodeLine::setColor (const osgVector4& color)
    {      
        color_ptr_->at(0) = color;
        beam_ptr_->dirtyDisplayList();
        Node::setAlpha(color.a());
    }

    void LeafNodeLine::setAlpha (const float& alpha)
    {
      color_ptr_->at(0).a() = alpha;
      beam_ptr_->dirtyDisplayList();
      Node::setAlpha(alpha);
    }

    void LeafNodeLine::setLineWidth (const float& width)
    {
      osg::LineWidth* linewidth = static_cast<osg::LineWidth*>(
        beam_ptr_->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINEWIDTH));
      linewidth->setWidth(width);
      beam_ptr_->dirtyDisplayList();
    }

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
    
} /* namespace graphics */
