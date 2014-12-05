//
//  LeafNodeCollada.h
//  SceneViewer
//
//  Created by Justin Carpentier, Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_LEAFNODECOLLADA_HH
#define SCENEVIEWER_LEAFNODECOLLADA_HH

#include "Node.h"
#include <osgDB/ReadFile>

namespace Graphics {
    DEF_CLASS_SMART_PTR(LeafNodeCollada)
    
    /** Implementation of the Collada GraphicalObject in OSG render */
    class LeafNodeCollada : public Node
    {
    private:

        std::string collada_file_path_;

        /** Associated weak pointer */
        LeafNodeColladaWeakPtr weak_ptr_;
        
        /** Associated Node Shape */
        ::osg::NodeRefPtr collada_ptr_;
        
        void init();
        
        /* Default constructor */
        LeafNodeCollada(const std::string& name, const std::string& collada_file_path);
        
        /* Copy constructor */
        LeafNodeCollada(const LeafNodeCollada& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr (LeafNodeColladaWeakPtr other_weak_ptr);
        
    protected:
    public:
        /** Static method which create a new LeafNodeCollada
         */
        static LeafNodeColladaPtr_t create(const std::string& name, const std::string& collada_file_path);
        
        /** Static method for creating a clone of LeafNodeCollada other
         */
        static LeafNodeColladaPtr_t createCopy(LeafNodeColladaPtr_t other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeColladaPtr_t clone(void) const;
        
        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual LeafNodeColladaPtr_t copy() const { return clone(); }
        
        /** Return a shared pointer of the current object
         */
        LeafNodeColladaPtr_t self(void) const;
        
        /** Destructor */
        virtual ~LeafNodeCollada();
        
    };
} /* namespace Graphics */

#endif /* SCENEVIEWER_LEAFNODECOLLADA_HH */
