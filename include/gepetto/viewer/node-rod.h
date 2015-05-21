//
//  node-rod.cpp
//  gepetto-viewer
//
//  Created by Pierre Fernbach in may 2015.
//  Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//


#ifndef ROD_HH
#define ROD_HH

#include <gepetto/viewer/group-node.h>
#include <gepetto/viewer/leaf-node-capsule.h>
#include <gepetto/viewer/config-osg.h>

namespace graphics {

    DEF_CLASS_SMART_PTR(NodeRod)

    class NodeRod : public Node
    {
        private :
            std::vector<LeafNodeCapsulePtr_t> list_of_capsule_;
            NodeRodWeakPtr weak_ptr_;
            void initWeakPtr(NodeRodWeakPtr other_weak_ptr);
            osgVector4 color_;
            float radius_;
            float totalLength_;
            short maxCapsule_; // max number of capsule for this rod
        protected:

            /** Default constructor */
            NodeRod(const std::string& name,osgVector4 color, float radius, float totalLength, int maxCapsule);

            /** Copy constructor */
            NodeRod (const NodeRod& other);


        public:
            /** Static method which create the new rod */
            static NodeRodPtr_t create(const std::string& name,osgVector4 color, float radius, float totalLength,short maxCapsule);

            /** Return the ID of the capsule at the index i */
            virtual std::string getCapsuleName(int i);

            virtual LeafNodeCapsulePtr_t getCapsule(size_t i) const;

            /** Static method for creating a clone of the rod other with the copy constructor        */
            static NodeRodPtr_t createCopy(NodeRodPtr_t other);


            /** Return a shared pointer of the current object      */
            NodeRodPtr_t self(void) const;

            virtual float radius() const{
                return radius_;
            }

            virtual float totalLength() const{
                return totalLength_;
            }

            virtual osgVector4 color() const{
                return color_;
            }

            virtual short maxCapsule() const{
                return maxCapsule_;
            }

    };
} //namespace graphics

#endif // ROD_HH
