//
//  group-node.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_GROUPNODE_HH
#define SCENEVIEWER_GROUPNODE_HH

#include <gepetto/viewer/node.h>

namespace graphics {
    
    DEF_CLASS_SMART_PTR(GroupNode)
    
    class GroupNode : public Node
    {
    private:
        /**
         \brief List of all child graphical object
         */
        std::list<graphics::NodePtr_t> list_of_objects_;
        
        /** Associated weak pointer */
        GroupNodeWeakPtr weak_ptr_;
        
    protected:
        /**
         \brief Default constructor
         */
        GroupNode (const std::string& name);
        
        /** Copy constructor */
        GroupNode (const GroupNode& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr(GroupNodeWeakPtr other_weak_ptr);
        
    public:
        /** Static method which create a new box defined by the half_axis vector
         */
       static GroupNodePtr_t create(const std::string& name);
        
        /** Static method for creating a clone of box other with the copy constructor
         */
        static GroupNodePtr_t createCopy(GroupNodePtr_t other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual GroupNodePtr_t clone(void) const;
        
        /** Return a shared pointer of the current object
         */
        GroupNodePtr_t self(void) const;
        
        
        /** Add a GraphicalObject to the list
         */
        virtual bool addChild (::graphics::NodePtr_t child_ptr);
        
        /** Remove a GraphicalObject from the list
         */
        virtual bool removeChild (::graphics::NodePtr_t child_ptr);
        
        /** Return true if this group contains this child
         */
        virtual bool hasChild (::graphics::NodePtr_t child_ptr);
        
        /** Remove all children
         */
        virtual void removeAllChildren();
        
        virtual size_t getNumOfChildren() const
        {
          return list_of_objects_.size();
        }

        virtual graphics::NodePtr_t getChild(size_t i) const
        {  
           std::list<graphics::NodePtr_t>::const_iterator it = list_of_objects_.begin();
           if (list_of_objects_.size() > i)
           {
               std::advance(it, i);
           }     
           return *it;    
        }
        /**
         \brief Virtual method for setting the visibility mode of the object : visible or not visible
         \param visibilitymode state
         */
        virtual void setVisibilityMode (const VisibilityMode& visibility_state);
        
        /**
         \brief Virtual method for setting the lighting mode of the object : influence by light or not
         \param lightingmode state
         */
        virtual void setLightingMode (const LightingMode& lighting_state);
        
        /**
         \brief Virtual method for setting the wireframe mode of the object : visible or not
         \param wireframemode state
         */
        virtual void setWireFrameMode (const WireFrameMode& wireframe_state);
        
        /** Replace the old transparency value with alpha
         *  Note : alpha must be in [0,1]
         */
        virtual void setAlpha (const float& alpha);
        
        /** Apply the scale value of all children
         * note : scale value must be positive
         */
        virtual void setScale (const float scale);

        /** Set the color of the object */
        void setColor (const osgVector4& color);

        void traverse (NodeVisitor& visitor);

	virtual osg::ref_ptr<osg::Node> getOsgNode() const;

        SCENE_VIEWER_ACCEPT_VISITOR;
        
        /* Destructor */
        virtual ~GroupNode();
        
    }; /* class GraphicalGroupOSG */

} /* namespace graphics */

#endif /* SCENEVIEWER_GROUPNODE_HH */
