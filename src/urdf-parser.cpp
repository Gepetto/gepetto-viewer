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

    void getStaticTransform (const boost::shared_ptr < urdf::Link >& link,
			     osgVector3 &static_pos, osgQuat &static_quat,
			     bool visual)
    {
      if (visual) {
	// Set staticTransform = transform from link to visual
	static_pos = osgVector3((float)link->visual->origin.position.x,
				(float)link->visual->origin.position.y,
				(float)link->visual->origin.position.z);

	static_quat=osgQuat( (float)link->visual->origin.rotation.x,
			     (float)link->visual->origin.rotation.y,
			     (float)link->visual->origin.rotation.z,
			     (float)link->visual->origin.rotation.w);
      } else {
	// Set staticTransform = transform from link to visual
	static_pos = osgVector3((float)link->collision->origin.position.x,
				(float)link->collision->origin.position.y,
				(float)link->collision->origin.position.z);

	static_quat=osgQuat( (float)link->collision->origin.rotation.x,
			     (float)link->collision->origin.rotation.y,
			     (float)link->collision->origin.rotation.z,
			     (float)link->collision->origin.rotation.w);
      }
    }

    void addMesh (const std::string &robotName,
                  const std::string &meshDataRootDir,
		  boost::shared_ptr < urdf::Link >& urdfLink,
		  GroupNodePtr_t &robot, bool visual)
    {
      std::string link_name;
      std::string mesh_path;
      ::boost::shared_ptr< ::urdf::Mesh > mesh_shared_ptr;

      if (visual) {
	mesh_shared_ptr = boost::static_pointer_cast< ::urdf::Mesh >
	  ( urdfLink->visual->geometry );
      } else {
	mesh_shared_ptr = boost::static_pointer_cast< ::urdf::Mesh >
	  ( urdfLink->collision->geometry );
      }
      link_name = urdfLink->name;
      std::cout << "Mesh " << std::endl;
      if ( mesh_shared_ptr != 0 )
        {
	  if (mesh_shared_ptr->filename.substr(0, 10) != "package://") {
	    throw std::runtime_error ("Error when parsing urdf file: "
				      "mesh filename should start with"
				      " \"package://\"");
	  }
          mesh_path =  mesh_shared_ptr->filename.substr
	    (10, mesh_shared_ptr->filename.size());
          LeafNodeColladaPtr_t link = LeafNodeCollada::create
	    (robotName + "/" + link_name, meshDataRootDir + mesh_path);
          osgVector3 static_pos; osgQuat static_quat;
          getStaticTransform (urdfLink, static_pos, static_quat, visual);
          link->setStaticTransform(static_pos,static_quat);
          link->setScale(osgVector3(mesh_shared_ptr->scale.x,
                                    mesh_shared_ptr->scale.y,
                                    mesh_shared_ptr->scale.z));
          // add links to robot node
          robot->addChild(link);
        }
    }

    void addCylinder (const std::string &robotName,
		      boost::shared_ptr < urdf::Link >& urdfLink,
		      GroupNodePtr_t &robot, bool visual)
    {
      std::string link_name;
      ::boost::shared_ptr< ::urdf::Cylinder > cylinder_shared_ptr;

      if (visual) {
	cylinder_shared_ptr = boost::static_pointer_cast < ::urdf::Cylinder >
	  ( urdfLink->visual->geometry );
      } else {
	cylinder_shared_ptr = boost::static_pointer_cast < ::urdf::Cylinder >
	  ( urdfLink->collision->geometry );
      }
      link_name = urdfLink->name;
      std::cout << "Cylinder" << std::endl;
      if ( cylinder_shared_ptr != 0 )
        {
          LeafNodeCylinderPtr_t link = LeafNodeCylinder::create( robotName + "/" + link_name,
                                                                 (float)cylinder_shared_ptr.get()->radius,
                                                                 (float)cylinder_shared_ptr.get()->length);
          osgVector3 static_pos; osgQuat static_quat;
          getStaticTransform (urdfLink, static_pos, static_quat, visual);
          link->setStaticTransform(static_pos,static_quat);

          // add links to robot node
          robot->addChild(link);
        }
    }

    void addBox (const std::string &robotName,
		 boost::shared_ptr < urdf::Link >& urdfLink,
		 GroupNodePtr_t &robot, bool visual)
    {
      std::string link_name;
      ::boost::shared_ptr< ::urdf::Box > box_shared_ptr;

      if (visual) {
	box_shared_ptr = boost::static_pointer_cast< ::urdf::Box >
	  ( urdfLink->visual->geometry);
      } else {
	box_shared_ptr = boost::static_pointer_cast< ::urdf::Box >
	  ( urdfLink->collision->geometry);
      }
      link_name = urdfLink->name;
      std::cout << "Box" << std::endl;
      if ( box_shared_ptr != 0 ) {
	LeafNodeBoxPtr_t link = LeafNodeBox::create
	  (robotName + "/" + link_name,
	   osgVector3(.5*(float)box_shared_ptr->dim.x,
		      .5*(float)box_shared_ptr->dim.y,
		      .5*(float)box_shared_ptr->dim.z));
          osgVector3 static_pos; osgQuat static_quat;
          getStaticTransform (urdfLink, static_pos, static_quat, visual);
          link->setStaticTransform(static_pos,static_quat);

          // add links to robot node
          robot->addChild(link);
        }
    }

    void addSphere (const std::string &robotName,
		    boost::shared_ptr < urdf::Link >& urdfLink,
		    GroupNodePtr_t &robot, bool visual)
    {
      std::string link_name;
      ::boost::shared_ptr< ::urdf::Sphere > sphere_shared_ptr;

      if (visual) {
	sphere_shared_ptr = boost::static_pointer_cast < ::urdf::Sphere >
	  ( urdfLink->visual->geometry );
      } else {
	sphere_shared_ptr = boost::static_pointer_cast < ::urdf::Sphere >
	  ( urdfLink->collision->geometry );
      }
      link_name = urdfLink->name;
      std::cout << "Sphere" << std::endl;
      if ( sphere_shared_ptr != 0 )
        {
          LeafNodeSpherePtr_t link = LeafNodeSphere::create( robotName + "/" + link_name,
                                                             (float)sphere_shared_ptr.get()->radius);
          osgVector3 static_pos; osgQuat static_quat;
          getStaticTransform (urdfLink, static_pos, static_quat, visual);
          link->setStaticTransform(static_pos,static_quat);

          // add links to robot node
          robot->addChild(link);
        }
    }

  }

  GroupNodePtr_t urdfParser::parse (const std::string& robotName,
				    const std::string& urdf_file_path,
				    const std::string& meshDataRootDir,
				    const std::string& collisionOrVisual)
  {
    if (collisionOrVisual != "visual" && collisionOrVisual != "collision") {
      throw std::runtime_error ("parameter collisionOrVisual should be either "
				"\"collision\" or \"visual\"");
    }
    boost::shared_ptr< urdf::ModelInterface > model =
      urdf::parseURDFFile( urdf_file_path );
    // Test that file has correctly been parsed
    if (!model) {
      throw std::runtime_error (std::string ("Failed to parse ") +
				urdf_file_path);
    }
    GroupNodePtr_t robot = GroupNode::create(robotName);
    std::vector< boost::shared_ptr < urdf::Link > > links;
    model->getLinks(links);
    std::string link_name;

    for (unsigned int i = 0 ; i < links.size() ; i++) {
      link_name = links[i]->name;
      std::cout << link_name << std::endl;

      if (collisionOrVisual == "visual") {
	if ( links[i]->visual != NULL && links[i]->visual->geometry != NULL) {
	  switch (links[i]->visual->geometry->type) {
	  case urdf::Geometry::MESH:
	    internal_urdf_parser::addMesh (robotName, meshDataRootDir,
					   links [i], robot, true);
	    break;
	  case urdf::Geometry::CYLINDER:
	    internal_urdf_parser::addCylinder (robotName, links [i], robot,
					       true);
	    break;
	  case urdf::Geometry::BOX:
	    internal_urdf_parser::addBox (robotName, links [i], robot, true);
	    break;
	  case urdf::Geometry::SPHERE:
	    internal_urdf_parser::addSphere (robotName, links [i], robot, true);
	    break;
	  }
	}
      } else {
	if ( links[i]->visual != NULL && links[i]->visual->geometry != NULL) {
	  switch (links[i]->collision->geometry->type) {
	  case urdf::Geometry::MESH:
	    internal_urdf_parser::addMesh (robotName, meshDataRootDir,
					   links [i], robot, false);
	    break;
	  case urdf::Geometry::CYLINDER:
	    internal_urdf_parser::addCylinder(robotName, links [i], robot,
					      false);
	    break;
	  case urdf::Geometry::BOX:
	    internal_urdf_parser::addBox (robotName, links [i], robot, false);
	    break;
	  case urdf::Geometry::SPHERE:
	    internal_urdf_parser::addSphere(robotName, links [i], robot, false);
	    break;
	  }
	}
      }
    }
    return robot;
  }
}
