//
//  leaf-node-collada.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_LEAFNODECOLLADA_HH
#define GEPETTO_VIEWER_LEAFNODECOLLADA_HH

#include <gepetto/viewer/node.h>
#include <osgDB/ReadFile>


namespace gepetto {
namespace viewer {
    DEF_CLASS_SMART_PTR(LeafNodeCollada)
    
    /** Implementation of the Collada GraphicalObject in OSG render */
    class LeafNodeCollada : public Node
    {
    private:

        std::string collada_file_path_;
        std::string texture_file_path_;

        /** Associated weak pointer */
        LeafNodeColladaWeakPtr weak_ptr_;
        
        /** Associated Node Shape */
        ::osg::GroupRefPtr group_ptr_;
        ::osg::NodeRefPtr collada_ptr_;
        
        void init();
        
        /* Default constructor */
        LeafNodeCollada(const std::string& name, const std::string& collada_file_path);
        LeafNodeCollada(const std::string& name, const std::string& collada_file_path, const osgVector4& color);
        LeafNodeCollada(const std::string& name, const ::osg::NodeRefPtr& node, const std::string& collada_file_path);
        /* Copy constructor */
        LeafNodeCollada(const LeafNodeCollada& other);
        
        /** Initialize weak_ptr */
        void initWeakPtr (LeafNodeColladaWeakPtr other_weak_ptr);

        //static void setColor(osg::NodeRefPtr osgNode_ptr,const osgVector4& color);
        
    protected:
    public:
        /** Static method which create a new LeafNodeCollada
         */
        static LeafNodeColladaPtr_t create(const std::string& name, ::osg::NodeRefPtr mesh, const std::string& collada_file_path);
        static LeafNodeColladaPtr_t create(const std::string& name, const std::string& collada_file_path);
        static LeafNodeColladaPtr_t create(const std::string& name, const std::string& collada_file_path, const osgVector4& color);
        
        /** Static method for creating a clone of LeafNodeCollada other
         */
        static LeafNodeColladaPtr_t createCopy(LeafNodeColladaPtr_t other);
        
        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual LeafNodeColladaPtr_t clone(void) const;
        
        /** Returns a pointer to the NodeRefPtr  */
        ::osg::NodeRefPtr getColladaPtr(void);

        /** Copy
         \brief Proceed to a copy of the currend object as clone
         */
        virtual LeafNodeColladaPtr_t copy() const { return clone(); }
        
        /** Return a shared pointer of the current object
         */
        LeafNodeColladaPtr_t self(void) const;
        
        void setColor(const osgVector4& color);   

        osgVector4 getColor () const;

        void setTexture(const std::string& image_path);     

        const std::string& meshFilePath () const;

        const std::string& textureFilePath () const;

	virtual void setAlpha(const float& alpha);
	virtual osg::ref_ptr<osg::Node> getOsgNode() const;

        SCENE_VIEWER_ACCEPT_VISITOR;

        /** Destructor */
        virtual ~LeafNodeCollada();
        
    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_LEAFNODECOLLADA_HH */
