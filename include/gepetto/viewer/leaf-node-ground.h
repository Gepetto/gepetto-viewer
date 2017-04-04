//
//  leaf-node-ground.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_LEAFNODEGROUND_HH
#define SCENEVIEWER_LEAFNODEGROUND_HH

#include <gepetto/viewer/node.h>

namespace graphics
{
    DEF_CLASS_SMART_PTR(LeafNodeGround)
    
    class LeafNodeGround : public Node
    {
    private:
      
        float length_;
        float width_;
        float square_length_;
        float square_width_;

        osgVector4 color1_;
        osgVector4 color2_;

        /** Associated weak pointer */
        LeafNodeGroundWeakPtr weak_ptr_;
      
        /** Array of colors */
        ::osg::Vec4ArrayRefPtr colors_array_ptr_;

        /** Associated Geometry for LeafNodeGround */
        ::osg::GeometryRefPtr grid_geometry_ptr_;
      
        /** Init method */
        void init();

        /** Constructor
         \brief Constructor with all useful parameters
         \param length : length of demi-axis
         \param width : width of demi-axis
         \param square_length : length of a basic square
         \param square_width : width of a basic square
         */
        LeafNodeGround(const std::string& name, const float& length, const float& width,
                  const float& square_length, const float& square_width, const osgVector4& color1, const osgVector4& color2);
        
        /** Copy constructor */
        LeafNodeGround(const LeafNodeGround& other);
      
        /** Initialize weak_ptr */
        void initWeakPtr( const LeafNodeGroundWeakPtr& other_weak_ptr );
        
    protected:
    public:
        
        /** Builder
         \param length : length of plane
         \param width : width of plane
         */
        static LeafNodeGroundPtr_t create(const std::string& name, const float &length, const float &width);
        
        /** Builder
         \brief Constructor with all useful parameters
         \param length : length of demi-axis
         \param width : width of demi-axis
         \param square_length : length of a basic square
         \param square_width : width of a basic square
         \param configuration_ptr : configuration of plane
         */
        static LeafNodeGroundPtr_t create(const std::string& name, const float &length, const float &width,
                                         const float &square_length, const float &square_width);

        static LeafNodeGroundPtr_t create(const std::string& name, const float &length, const float &width, const float &square_length, const float &square_width, const osgVector4& color1, const osgVector4& color2);

        static LeafNodeGroundPtr_t create(const std::string& name);
        
        /** Create copy */
        static LeafNodeGroundPtr_t createCopy( const LeafNodeGroundPtr_t &other );
        
        /** Clone
         \brief Returns a shared pointer to a newly allocated copy of the object.
         */
        virtual LeafNodeGroundPtr_t clone(void) const;
      
      
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual LeafNodeGroundPtr_t copy() const { return clone(); }
      
        /**
         \brief Returns a shared pointer of the object itself.
         */
        LeafNodeGroundPtr_t self(void) const;

        /**
         * \sa setColor1
         */
        void setColor (const osgVector4 &color);

        /** inline virtual void setColor1(const ColotSharedPtr &color1_ptr)
         \brief Allow to define color1
         \param color1_ptr : color to apply to first color
         */
        virtual void setColor1(const osgVector4 &color1);

        const osgVector4& getColor1() const
        {
            return color1_;
        }
        
        /** inline virtual void setColor2(const ColotSharedPtr &color2_ptr)
         \brief Allow to define color2
         \param color2_ptr : color to apply to second color
         */
        virtual void setColor2(const osgVector4 &color2);
        
        const osgVector4& getColor2() const
        {
            return color2_;
        }
        /** virtual void setColors(const ColorSharedPtr &color1_ptr , const ColorSharedPtr &color2_ptr);
         \brief Allow to define both colors
         \param color1_ptr : color to apply to first color
         \param color2_ptr : color to apply to second color
         */
        virtual void setColors(const osgVector4 &color1 , const osgVector4 &color2);
        
        /**
         \brief Virtual method for setting the shadow mode of the object : receive, cast, both or no shadow
         \param shadow_state state
         */
        //virtual void setShadowMode (const ShadowMode& shadow_state) {
        //    Node::setShadowMode ( shadow_state );
        //}

        SCENE_VIEWER_ACCEPT_VISITOR;
        
        /** Destructor */
        virtual ~LeafNodeGround();
    };
}

#endif /* SCENEVIEWER_LEAFNODEGROUND_HH */
