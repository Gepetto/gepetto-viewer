//
//  LeafNodeFace.cpp
//  SceneViewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <Graphics/FaceOSG.h>

#include <Graphics/conversionTools.h>

#include <osgUtil/SmoothingVisitor>
#include <osg/CullFace>
#include <osg/LineWidth>

namespace Graphics {
    
    /* Declaration of private function members */
    
    void FaceOSG::init ()
    {
        /* Set face Geometry */
        face_ptr_ = new ::osg::Geometry();
        
        /* Create Geode for adding geometry */
        geode_face_ptr_ = new osg::Geode();
        geode_face_ptr_->addDrawable(face_ptr_);
        
        ::osg::Vec3ArrayRefPtr vertices = new ::osg::Vec3Array;
        face_ptr_->setVertexArray( vertices );
        
        /* Define the face color */
        ::osg::Vec4ArrayRefPtr color_ptr = new ::osg::Vec4Array;
        face_ptr_->setColorArray(color_ptr);
        face_ptr_->setColorBinding(::osg::Geometry::BIND_OVERALL);
        
        /* Set edge Geometry */
        edge_ptr_ = new ::osg::Geometry();
        
        /* Create Geode for adding geometry */
        geode_edge_ptr_ = new osg::Geode();
        geode_edge_ptr_->addDrawable(edge_ptr_);
        
        edge_ptr_->setVertexArray( vertices );
        
        /* Define the edge color */
        ::osg::Vec4ArrayRefPtr edge_color_ptr = new ::osg::Vec4Array;
        edge_color_ptr->push_back(toOSGVector4( getEdgeColor()->asVector() ));
        edge_ptr_->setColorArray(edge_color_ptr);
        edge_ptr_->setColorBinding(::osg::Geometry::BIND_OVERALL);
        edge_ptr_->getOrCreateStateSet()->setMode(GL_LIGHTING, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
        
        /* Define Switch Node */
        switch_ref_ptr_ = new ::osg::Switch;
        switch_ref_ptr_->addChild(geode_face_ptr_, true);
        switch_ref_ptr_->addChild(geode_edge_ptr_, false);
        
        /* Create PositionAttitudeTransform */
        this->asQueue()->addChild(switch_ref_ptr_);
        
        /* Allow transparency */
        geode_face_ptr_->getOrCreateStateSet()->setRenderBinDetails(10, "DepthSortedBin");
        geode_face_ptr_->getStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);
        
        geode_edge_ptr_->getOrCreateStateSet()->setRenderBinDetails(12, "DepthSortedBin");
        geode_edge_ptr_->getStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::ON);
        
        /* Apply color */
        setFaceColor(getFaceColor());
        setEdgeColor(getEdgeColor());
        
        setFaceMode(getFaceMode());

        createGeometry();
    }
    
    FaceOSG::FaceOSG() :
        Graphics::GraphicalObjectOSG(),
        Graphics::Face()
    {
        init();
    }
    
    FaceOSG::FaceOSG(const FaceOSG& other) :
        Graphics::GraphicalObjectOSG(other),
        Graphics::Face(other)
    {
        init();
        
    }
    
    void FaceOSG::createGeometry ()
    {
        ::osg::Vec3ArrayRefPtr face_vertices = new ::osg::Vec3Array;
        
        /* Populate vector */
        size_t i;
        for (i=0;
             i < getNumberOfVertices();
             i++)
        {
            face_vertices->push_back( toOSGVector3( getVertex(i) ) );
        }
        
        if (getNumberOfVertices() > 2)
            face_vertices->push_back( toOSGVector3( getVertex(0) ) );
        
        /* Apply vertices */
        face_ptr_->setVertexArray( face_vertices );
        edge_ptr_->setVertexArray( face_vertices );
        
        //face_ptr_->addPrimitiveSet( new ::osg::DrawArrays( GL_POLYGON, 0, (int)getNumberOfVertices() ) );
        face_ptr_->addPrimitiveSet( new ::osg::DrawArrays() );
        //edge_ptr_->addPrimitiveSet( new ::osg::DrawArrays( GL_LINE_STRIP, 0, 0 ) );
        edge_ptr_->addPrimitiveSet( new ::osg::DrawArrays() );
        
        updateGeometry();
    }
    
    void FaceOSG::updateGeometry ()
    {
        switch (getFaceMode()) {
            case ::Graphics::FACE:
                updateFaceGeometry();
                break;
                
            case ::Graphics::EDGE:
                updateEdgeGeometry();
                break;
                
            case ::Graphics::FACE_AND_EDGE:
                updateFaceGeometry();
                updateEdgeGeometry();
                break;
                
            default:
                ASSERT(false, "FaceMode set with bad option");
                break;
        }
    }
    
    void FaceOSG::updateFaceGeometry ()
    {
        face_ptr_->setPrimitiveSet(0, new ::osg::DrawArrays( GL_POLYGON, 0, (int)getNumberOfVertices() ) );
        
        /* Tessellator */
        osgUtil::Tessellator tessellator;
        tessellator.retessellatePolygons( *(face_ptr_.get()) );
        
        /* Smoothing Visitor */
        osgUtil::SmoothingVisitor::smooth( *(face_ptr_.get()) );
    }
    
    void FaceOSG::updateEdgeGeometry ()
    {
        if ( getNumberOfVertices() == 2 )
            edge_ptr_->setPrimitiveSet(0, new ::osg::DrawArrays( GL_LINE_STRIP, 0, (int)getNumberOfVertices() ) ); // just a line
        else if ( getNumberOfVertices() > 2 )
            edge_ptr_->setPrimitiveSet(0, new ::osg::DrawArrays( GL_LINE_STRIP, 0, (int)getNumberOfVertices() + 1 ) ); // more than 2 vertices
        else
            edge_ptr_->setPrimitiveSet(0, new ::osg::DrawArrays( GL_LINE_STRIP, 0, 0 ) ); // only one vertex
    }
    
    void FaceOSG::initWeakPtr(FaceOSGWeakPtr other_weak_ptr)
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */
    
    FaceOSGSharedPtr FaceOSG::create ()
    {
        FaceOSGSharedPtr shared_ptr(new FaceOSG());
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }
    
    FaceOSGSharedPtr FaceOSG::createCopy (FaceOSGSharedPtr other)
    {
        FaceOSGSharedPtr shared_ptr(new FaceOSG(*other));
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }
    
    /* End of declaration of protected function members */
    
    /* Declaration of public function members */
    
    FaceOSGSharedPtr FaceOSG::clone (void) const
    {
        return FaceOSG::createCopy(weak_ptr_.lock());
    }
    
    FaceOSGSharedPtr FaceOSG::self (void) const
    {
        return weak_ptr_.lock();
    }
    
    void FaceOSG::setFaceMode(const FaceMode& face_mode)
    {
        Face::setFaceMode(face_mode);
        
        switch (face_mode) {
            case ::Graphics::FACE:
                switch_ref_ptr_->setValue (0, true);
                switch_ref_ptr_->setValue (1, false);
                break;
                
            case ::Graphics::EDGE:
                switch_ref_ptr_->setValue (0, false);
                switch_ref_ptr_->setValue (1, true);
                break;
                
            case ::Graphics::FACE_AND_EDGE:
                switch_ref_ptr_->setValue (0, true);
                switch_ref_ptr_->setValue (1, true);
                break;
                
            default:
                ASSERT(false, "FaceMode set with bad option");
                break;
        }
        
        //updateGeometry();
    }
    
    bool FaceOSG::addVertex (const DefVector3& vertex)
    {
        return addVertex(getNumberOfVertices(), vertex);
    }
    
    bool FaceOSG::addVertex (unsigned int i, const DefVector3& vertex)
    {
        if ( Face::addVertex(i, vertex))
        {
            ::osg::Vec3ArrayRefPtr vertices = static_cast<::osg::Vec3Array*>( face_ptr_->getVertexArray() );
            ::osg::Vec3Array::iterator iter_vertices = vertices->begin();
            
            ::std::advance(iter_vertices, i);
            
            if (getNumberOfVertices() <= 2)
            {
                vertices->insert(iter_vertices, 1, toOSGVector3(vertex));
            }
            else
            {
                if (getNumberOfVertices() > 3)
                    vertices->pop_back();
                
                vertices->insert(iter_vertices, 1, toOSGVector3(vertex));
                vertices->push_back( toOSGVector3( getVertex(0) ) );
            }
            
            updateGeometry();
            
            return true;
        }
        
        return false;
    }
    
    bool FaceOSG::removeVertex (size_t i)
    {
        if ( Face::removeVertex(i) )
        {
            ::osg::Vec3ArrayRefPtr vertices = static_cast<::osg::Vec3Array*>( face_ptr_->getVertexArray() );
            ::osg::Vec3Array::iterator iter_vertices = vertices->begin();
            
            ::std::advance(iter_vertices, i);
            vertices->erase(iter_vertices);
            
            if (i==0 && getNumberOfVertices() >= 2)
            {
                vertices->pop_back();
                if (getNumberOfVertices() >= 3)
                {
                    vertices->push_back( toOSGVector3( getVertex(0) ) );
                }
            }
            
            updateGeometry();
            
            return true;
        }
        
        return false;
    }
    
    void FaceOSG::removeAllVertices ()
    {
        Face::removeAllVertices();
        ::osg::Vec3ArrayRefPtr vertices = static_cast<::osg::Vec3Array*>( face_ptr_->getVertexArray() );
        vertices->erase(vertices->begin(), vertices->end());
        
        updateGeometry();
    }
    
    void FaceOSG::setFaceColor (ColorConstSharedPtr face_color_ptr)
    {
        Face::setFaceColor(face_color_ptr);
        
        ::osg::Vec4ArrayRefPtr color_array_ptr = dynamic_cast<::osg::Vec4Array*>(face_ptr_->getColorArray());
        ASSERT(color_array_ptr, "Problem of dynamic casting from VecArray to Vec4Array");
        
        color_array_ptr->at(0) = toOSGVector4(face_color_ptr->asVector());
    }
    
    void FaceOSG::setEdgeColor (ColorConstSharedPtr edge_color_ptr)
    {
        Face::setEdgeColor(edge_color_ptr);
        
        ::osg::Vec4ArrayRefPtr color_array_ptr = dynamic_cast<::osg::Vec4Array*>(edge_ptr_->getColorArray());
        ASSERT(color_array_ptr, "Problem of dynamic casting from VecArray to Vec4Array");
        
        color_array_ptr->at(0) = toOSGVector4(edge_color_ptr->asVector());
    }
    
    void FaceOSG::setAlpha (const DefScalar& alpha)
    {
        Face::setAlpha(alpha);
        
        ::osg::Vec4ArrayRefPtr color_array_ptr = dynamic_cast<::osg::Vec4Array*>(face_ptr_->getColorArray());
        ASSERT(color_array_ptr, "Problem of dynamic casting from VecArray to Vec4Array");
        
        color_array_ptr->at(0) = toOSGVector4(getFaceColor()->asVector());
    }
    
    void FaceOSG::setScale (const DefScalar& scale)
    {
        GraphicalObject::setScale(scale);
        GraphicalObjectOSG::setScale(scale);
    }
    
    FaceOSG::~FaceOSG ()
    {
        /* Proper deletion of all tree scene */
        geode_face_ptr_->removeDrawable(face_ptr_);
        face_ptr_ = NULL;
        
        switch_ref_ptr_->removeChild(geode_face_ptr_);
        switch_ref_ptr_->removeChild(geode_edge_ptr_);
        geode_face_ptr_ = NULL;
        geode_edge_ptr_ = NULL;
        
        this->asQueue()->removeChild(switch_ref_ptr_);
        switch_ref_ptr_ = NULL;
        
        weak_ptr_.reset();
    }
    
    /* End of declaration of public function members */
    
} /* namespace Graphics */
