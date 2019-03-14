//
//  leaf-node-line.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_LEAFNODELINE_HH
#define GEPETTO_VIEWER_LEAFNODELINE_HH

#include <gepetto/viewer/node-drawable.h>

namespace gepetto {
namespace viewer {
    DEF_CLASS_SMART_PTR(LeafNodeLine)
    
    class LeafNodeLine : public NodeDrawable
    {
    private:

        /** Associated weak pointer */
        LeafNodeLineWeakPtr weak_ptr_;
        
        /** Geometry */
        ::osg::GeometryRefPtr beam_ptr_;
        ::osg::ref_ptr< ::osg::DrawArrays > drawArray_ptr_;
        ::osg::Vec3ArrayRefPtr points_ptr_;
        ::osg::Vec4ArrayRefPtr color_ptr_;
        
        void init ();
        
        /* Default constructor */
        LeafNodeLine (const std::string& name, const osgVector3& start_point, const osgVector3& end_point);
        LeafNodeLine (const std::string& name, const osgVector3& start_point, const osgVector3& end_point, const osgVector4& color);
        LeafNodeLine (const std::string& name, const ::osg::Vec3ArrayRefPtr& points, const osgVector4& color);
        
        /* Copy constructor */
        LeafNodeLine (const LeafNodeLine& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr (LeafNodeLineWeakPtr other_weak_ptr);
        
    protected:
    public:
        /** Static method which create a new box defined by the half_axis vector
         */
        static LeafNodeLinePtr_t create (const std::string& name, const osgVector3& start_point, const osgVector3& end_point);
        static LeafNodeLinePtr_t create (const std::string& name, const osgVector3& start_point, const osgVector3& end_point, const osgVector4& color);
        static LeafNodeLinePtr_t create (const std::string& name, const ::osg::Vec3ArrayRefPtr& points, const osgVector4& color);

        /** Static method for creating a clone of box other with the copy constructor
         */
        static LeafNodeLinePtr_t createCopy (LeafNodeLinePtr_t other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeLinePtr_t clone (void) const;
        
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual NodePtr_t copy() const { return clone(); }
        
        /** Return a shared pointer of the current object
         */
        LeafNodeLinePtr_t self (void) const;
        
        /** Apply a new global configuration
         */
        
        /** Define the new start point of the line
         */
        virtual void setStartPoint (const osgVector3& start_point);

        osgVector3 getStartPoint () const;
        
        /** Define the new end point of the line
         */
        virtual void setEndPoint (const osgVector3& end_point);
        
        osgVector3 getEndPoint() const;

        /** Define the immediate mode of OpenGL used to draw the line.
         *  Possible values are:
         *  \image html opengl_primitives.png
         */
        virtual void setMode (const GLenum& mode);

        GLenum getMode () const;

        /** Define the start and end point of the line
         */
        virtual void setPoints (const osgVector3& start_point, const osgVector3& end_point);

        virtual void setPoints (const ::osg::Vec3ArrayRefPtr& points);

        /** Draw only a subset of the points
         */
        void setPointsSubset (const int first, const std::size_t count);

        ::osg::Vec3ArrayRefPtr getPoints()
        {
          return points_ptr_;
        }

        void setColor(const osgVector4& color);
        void setColors(const ::osg::Vec4ArrayRefPtr& color);

        osgVector4 getColor() const
        {
            ::osg::Vec4ArrayRefPtr color_array_ptr = dynamic_cast<osg::Vec4Array*>(beam_ptr_->getColorArray());
            ASSERT(color_array_ptr, "Problem of dynamic casting from VecArray to Vec4Array");

            return color_array_ptr->at(0);
        }

        ::osg::GeometryRefPtr geometry () const
        {
          return beam_ptr_;
        }

        SCENE_VIEWER_ACCEPT_VISITOR;

        /** Destructor */
        virtual ~LeafNodeLine();
        
    };
    
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LEAFNODELINE_HH */
