//
//  urdf-parser.cpp
//  gepetto-viewer
//
//  Created by Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/urdf-parser.h>
#include <gepetto/viewer/leaf-node-cylinder.h>
#include <gepetto/viewer/leaf-node-box.h>
#include <gepetto/viewer/leaf-node-sphere.h>

namespace graphics {

  namespace internal_urdf_parser
  {

    void setStaticTransform(std::vector< boost::shared_ptr < urdf::Link > > &links, 
                            int i,osgVector3 &static_pos, osgQuat &static_quat )
    {
      // Set staticTransform = transform from link to visual
      static_pos = osgVector3((float)links[i]->visual->origin.position.x, 
                              (float)links[i]->visual->origin.position.y, 
                              (float)links[i]->visual->origin.position.z);
    
      static_quat=osgQuat( (float)links[i]->visual->origin.rotation.x, 
                           (float)links[i]->visual->origin.rotation.y, 
                           (float)links[i]->visual->origin.rotation.z, 
                           (float)links[i]->visual->origin.rotation.w);
    }

    void addMesh(const std::string &robotName,
                 const std::string &package_path,
                 std::vector< boost::shared_ptr < urdf::Link > > &links, 
                 int i,
                 GroupNodePtr_t &robot)
    {
      std::string link_name;
      std::string mesh_path;
      ::boost::shared_ptr< ::urdf::Mesh > mesh_shared_ptr;

      mesh_shared_ptr = ::boost::static_pointer_cast< ::urdf::Mesh > ( links[i]->visual->geometry );
      link_name = links[i]->name;
      std::cout << "Mesh " << std::endl;
      if ( mesh_shared_ptr != 0 ) 
        {
          mesh_path =  mesh_shared_ptr->filename.substr(10, mesh_shared_ptr->filename.size());
          LeafNodeColladaPtr_t link = LeafNodeCollada::create( robotName + "/" + link_name, package_path + mesh_path);
          osgVector3 static_pos; osgQuat static_quat;
          setStaticTransform(links,i,static_pos,static_quat);
          link->setStaticTransform(static_pos,static_quat);
          link->setScale(osgVector3(mesh_shared_ptr->scale.x,
                                    mesh_shared_ptr->scale.y,
                                    mesh_shared_ptr->scale.z));
          // add links to robot node
          robot->addChild(link);
        }
    }

    void addCylinder(const std::string &robotName,
                     std::vector< boost::shared_ptr < urdf::Link > > &links, 
                     int i,
                     GroupNodePtr_t &robot)
    {
      std::string link_name;
      ::boost::shared_ptr< ::urdf::Cylinder > cylinder_shared_ptr;

      cylinder_shared_ptr = ::boost::static_pointer_cast< ::urdf::Cylinder > ( links[i]->visual->geometry );
      link_name = links[i]->name;
      std::cout << "Cylinder" << std::endl;
      if ( cylinder_shared_ptr != 0 ) 
        {
          LeafNodeCylinderPtr_t link = LeafNodeCylinder::create( robotName + "/" + link_name, 
                                                                 (float)cylinder_shared_ptr.get()->radius,
                                                                 (float)cylinder_shared_ptr.get()->length);
          osgVector3 static_pos; osgQuat static_quat;
          setStaticTransform(links,i,static_pos,static_quat);
          link->setStaticTransform(static_pos,static_quat);
        
          // add links to robot node
          robot->addChild(link);
        }
    }
  
    void addBox(const std::string &robotName,
                std::vector< boost::shared_ptr < urdf::Link > >&links, 
                int i,
                GroupNodePtr_t &robot)
    {
      std::string link_name;
      ::boost::shared_ptr< ::urdf::Box > box_shared_ptr;

      box_shared_ptr = ::boost::static_pointer_cast< ::urdf::Box > ( links[i]->visual->geometry );
      link_name = links[i]->name;
      std::cout << "Box" << std::endl;
      if ( box_shared_ptr != 0 ) 
        {
          LeafNodeBoxPtr_t link = LeafNodeBox::create( robotName + "/" + link_name, 
                                                       osgVector3((float)box_shared_ptr.get()->dim.x,
                                                                  (float)box_shared_ptr.get()->dim.y,
                                                                  (float)box_shared_ptr.get()->dim.z));
          osgVector3 static_pos; osgQuat static_quat;
          setStaticTransform(links,i,static_pos,static_quat);
          link->setStaticTransform(static_pos,static_quat);
        
          // add links to robot node
          robot->addChild(link);
        }
    }

    void addSphere(const std::string &robotName,
                   std::vector< boost::shared_ptr < urdf::Link > >&links, 
                   int i,
                   GroupNodePtr_t &robot)
    {
      std::string link_name;
      ::boost::shared_ptr< ::urdf::Sphere > sphere_shared_ptr;

      sphere_shared_ptr = ::boost::static_pointer_cast< ::urdf::Sphere > ( links[i]->visual->geometry );
      link_name = links[i]->name;
      std::cout << "Sphere" << std::endl;
      if ( sphere_shared_ptr != 0 ) 
        {
          LeafNodeSpherePtr_t link = LeafNodeSphere::create( robotName + "/" + link_name, 
                                                             (float)sphere_shared_ptr.get()->radius);
          osgVector3 static_pos; osgQuat static_quat;
          setStaticTransform(links,i,static_pos,static_quat);
          link->setStaticTransform(static_pos,static_quat);
        
          // add links to robot node
          robot->addChild(link);
        }
    }
  
  }

  GroupNodePtr_t urdfParser::parse(const std::string& robotName, const std::string& urdf_file_path, const std::string& package_path)
  {
    boost::shared_ptr< urdf::ModelInterface > model = urdf::parseURDFFile( urdf_file_path );
    GroupNodePtr_t robot = GroupNode::create(robotName);
    std::vector< boost::shared_ptr < urdf::Link > > links;
    model->getLinks(links);
    std::string link_name;
    
    for (unsigned int i = 0 ; i < links.size() ; i++) {
      link_name = links[i]->name;
      std::cout << link_name << std::endl;
      
      if ( links[i]->visual != NULL && links[i]->visual->geometry != NULL)
        {
          switch (links[i]->visual->geometry->type) {
          case urdf::Geometry::MESH:	
            internal_urdf_parser::addMesh(robotName,package_path,links,i,robot);
            break;
          case urdf::Geometry::CYLINDER:
            internal_urdf_parser::addCylinder(robotName,links,i,robot);
            break;
          case urdf::Geometry::BOX:
            internal_urdf_parser::addBox(robotName,links,i,robot);
            break;
          case urdf::Geometry::SPHERE:
            internal_urdf_parser::addSphere(robotName,links,i,robot);
            break;

          }
      
        }
    }

    return robot;
    
  }
}
