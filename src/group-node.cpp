//
//  group-node.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/group-node.h>

namespace graphics {
    
    /* Declaration of private function members */
    
    GroupNode::GroupNode (const std::string& name):
        graphics::Node (name), list_of_objects_()
    {
    }
    
    GroupNode::GroupNode (const GroupNode& other):
        graphics::Node (other), list_of_objects_()
    {        
        size_t i;
        for (i=0;
             i < other.getNumOfChildren();
             i++)
        {
            addChild(other.getChild(i));
        }
    }
    
    void GroupNode::initWeakPtr(GroupNodeWeakPtr other_weak_ptr)
    {
        weak_ptr_ = other_weak_ptr;
    }
    
    /* End of declaration of private function members */
    
    /* Declaration of protected function members */
    GroupNodePtr_t GroupNode::create(const std::string& name)
    {
        GroupNodePtr_t shared_ptr(new GroupNode(name));
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }
    
    GroupNodePtr_t GroupNode::createCopy(GroupNodePtr_t other)
    {
        GroupNodePtr_t shared_ptr(new GroupNode(*other));
        
        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);
        
        return shared_ptr;
    }
    
    /* End of declaration of protected function members */
    
    /* Declaration of public function members */
    
    GroupNodePtr_t GroupNode::clone(void) const
    {
        return GroupNode::createCopy(weak_ptr_.lock());
    }
    
    GroupNodePtr_t GroupNode::self(void) const
    {
        return weak_ptr_.lock();
    }
    
    bool GroupNode::addChild (NodePtr_t child_ptr)
    {
        list_of_objects_.push_back(child_ptr);
        this->asQueue()->addChild(child_ptr->asGroup());
        return true;
    }
    
    bool GroupNode::removeChild (NodePtr_t child_ptr)
    {
        list_of_objects_.remove(child_ptr);
        return this->asQueue()->removeChild(this->asQueue()->getChildIndex(child_ptr->asGroup()));
    }

    bool GroupNode::hasChild (::graphics::NodePtr_t child_ptr)
    {
        std::list<graphics::NodePtr_t>::const_iterator it =
            std::find (list_of_objects_.begin(), list_of_objects_.end(), child_ptr);
        return it != list_of_objects_.end();
    }
    
    void GroupNode::removeAllChildren()
    {
        list_of_objects_.clear();
        this->asQueue()->removeChild(0, this->asQueue()->getNumChildren());
    }
    
    void GroupNode::setVisibilityMode (const VisibilityMode& visibility_state)
    {
        Node::setVisibilityMode (visibility_state);
        std::list<graphics::NodePtr_t>::iterator iter_list_of_objects;
        for (iter_list_of_objects = list_of_objects_.begin();
             iter_list_of_objects != list_of_objects_.end();
             iter_list_of_objects++)
        {
            (*iter_list_of_objects)->setVisibilityMode ( visibility_state );
        }
    }
    
    void GroupNode::setLightingMode (const LightingMode& lighting_state)
    {
        Node::setLightingMode ( lighting_state );
        ::std::list<NodePtr_t>::iterator iter_list_of_objects;
        for (iter_list_of_objects = list_of_objects_.begin();
             iter_list_of_objects != list_of_objects_.end();
             iter_list_of_objects++)
        {
            (*iter_list_of_objects)->setLightingMode ( lighting_state );
        }
    }
    
    void GroupNode::setWireFrameMode (const WireFrameMode& wireframe_state)
    {
        Node::setWireFrameMode ( wireframe_state );
        
        ::std::list<NodePtr_t>::iterator iter_list_of_objects;
        for (iter_list_of_objects = list_of_objects_.begin();
             iter_list_of_objects != list_of_objects_.end();
             iter_list_of_objects++)
        {
            (*iter_list_of_objects)->setWireFrameMode ( wireframe_state );
        }
    }
    
    /*void GroupNode::setAlpha (const float alpha)
    {
        Node::setAlpha (alpha);
        ::std::list<NodePtr_t>::iterator iter_list_of_objects;
        for (iter_list_of_objects = list_of_objects_.begin();
             iter_list_of_objects != list_of_objects_.end();
             iter_list_of_objects++)
        {
            (*iter_list_of_objects)->setAlpha ( alpha );
        }
    }*/
    
    void GroupNode::setScale (const float scale)
    {
        GroupNode::setScale (scale);
        ::std::list<NodePtr_t>::iterator iter_list_of_objects;
        for (iter_list_of_objects = list_of_objects_.begin();
             iter_list_of_objects != list_of_objects_.end();
             iter_list_of_objects++)
        {
            (*iter_list_of_objects)->setScale ( scale );
        }
    }
    
    GroupNode::~GroupNode()
    {
        removeAllChildren();
    }
    
    /* End of declaration of public function members */
    
} /* namespace graphics */
