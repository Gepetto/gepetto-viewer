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
    std::string getFilename (const std::string& input, const std::string& meshDataRootDir)
    {
      if (input.substr(0, 10) == "package://") {
        return meshDataRootDir + "/" + input.substr(10, std::string::npos);
      } else {
        return input;
      }
    }

    void getStaticTransform (const boost::shared_ptr < urdf::Link >& link,
			     osgVector3 &static_pos, osgQuat &static_quat,
			     bool visual, long unsigned int visual_index)
    {
      if (visual || (link->visual_array.size()>1))
	{
	  if (link->visual_array.size()>1)
	    {
	      // Set staticTransform = transform from link to visual
	      static_pos = osgVector3((float)link->visual_array[visual_index]->origin.position.x,
				      (float)link->visual_array[visual_index]->origin.position.y,
				      (float)link->visual_array[visual_index]->origin.position.z);
	      
	      static_quat=osgQuat( (float)link->visual_array[visual_index]->origin.rotation.x,
				   (float)link->visual_array[visual_index]->origin.rotation.y,
				   (float)link->visual_array[visual_index]->origin.rotation.z,
				   (float)link->visual_array[visual_index]->origin.rotation.w);
	    }
	  else
	    {
	      // Set staticTransform = transform from link to visual
	      static_pos = osgVector3((float)link->visual->origin.position.x,
				      (float)link->visual->origin.position.y,
				      (float)link->visual->origin.position.z);
	      
	      static_quat=osgQuat( (float)link->visual->origin.rotation.x,
				   (float)link->visual->origin.rotation.y,
				   (float)link->visual->origin.rotation.z,
				   (float)link->visual->origin.rotation.w);
	    }
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
		  std::size_t j,
		  GroupNodePtr_t &linkNode, bool visual, bool linkFrame)
    {
      std::string link_name;
      std::string mesh_path;
      ::boost::shared_ptr< ::urdf::Mesh > mesh_shared_ptr;

      if (visual) {
	mesh_shared_ptr = boost::static_pointer_cast< ::urdf::Mesh >
	  ( urdfLink->visual_array [j]->geometry );
      } else {
	mesh_shared_ptr = boost::static_pointer_cast< ::urdf::Mesh >
	  ( urdfLink->collision_array [j]->geometry );
      }
      link_name = urdfLink->name;
      if ( mesh_shared_ptr != 0 )
        {
          mesh_path = getFilename (mesh_shared_ptr->filename, meshDataRootDir);
	  std::ostringstream oss;
	  oss << robotName << "/" << link_name << "_" << j;
          LeafNodeColladaPtr_t meshNode = LeafNodeCollada::create
	    (oss.str (), mesh_path);
          osgVector3 static_pos; osgQuat static_quat;
	  if (linkFrame) {
	    getStaticTransform (urdfLink, static_pos, static_quat, visual,j);
	  }
          meshNode->setStaticTransform(static_pos,static_quat);
          meshNode->setScale(osgVector3((float)mesh_shared_ptr->scale.x,
                                    (float)mesh_shared_ptr->scale.y,
                                    (float)mesh_shared_ptr->scale.z));

	  linkNode->addChild (meshNode);
          // Set Color if specified
          if (visual && urdfLink->visual_array [j]->material != NULL) {
            osgVector4 color(urdfLink->visual_array [j]->material->color.r, 
			     urdfLink->visual_array [j]->material->color.g, 
			     urdfLink->visual_array [j]->material->color.b, 
			     urdfLink->visual_array [j]->material->color.a);
            meshNode->setColor(color);
            if (urdfLink->visual_array [j]->material->texture_filename != "") {
              std::string textureFilename = getFilename
                (urdfLink->visual_array [j]->material->texture_filename, meshDataRootDir);
              meshNode->setTexture(textureFilename);
            }
          }
        }
    }

    void addCylinder (const std::string &robotName,
                      const std::string& meshDataRootDir,
		      boost::shared_ptr < urdf::Link >& urdfLink,
		      std::size_t j,
		      GroupNodePtr_t &linkNode, bool visual, bool linkFrame)
    {
      std::string link_name;
      ::boost::shared_ptr< ::urdf::Cylinder > cylinder_shared_ptr;

      if (visual) {
	cylinder_shared_ptr = boost::static_pointer_cast < ::urdf::Cylinder >
	  ( urdfLink->visual_array [j]->geometry );
      } else {
	cylinder_shared_ptr = boost::static_pointer_cast < ::urdf::Cylinder >
	  ( urdfLink->collision_array [j]->geometry );
      }
      link_name = urdfLink->name;
      std::cout << "Cylinder" << std::endl;
      if ( cylinder_shared_ptr != 0 )
        {
	  std::ostringstream oss;
	  oss << robotName << "/" << link_name << "_" << j;
          LeafNodeCylinderPtr_t cylinderNode
	    (LeafNodeCylinder::create
	     (oss.str (), (float)cylinder_shared_ptr.get()->radius,
	      (float)cylinder_shared_ptr.get()->length));
          osgVector3 static_pos; osgQuat static_quat;
	  if (linkFrame) {
	    getStaticTransform (urdfLink, static_pos, static_quat, visual,j);
	  }
          cylinderNode->setStaticTransform(static_pos,static_quat);

          // Set Color if specified
          if (visual && urdfLink->visual_array [j]->material != NULL) {
            osgVector4 color(urdfLink->visual_array [j]->material->color.r, urdfLink->visual_array [j]->material->color.g, urdfLink->visual_array [j]->material->color.b, urdfLink->visual_array [j]->material->color.a);
            cylinderNode->setColor(color);
            if (urdfLink->visual_array [j]->material->texture_filename != "") {
              std::string textureFilename = getFilename
                (urdfLink->visual_array [j]->material->texture_filename, meshDataRootDir);
              cylinderNode->setTexture(textureFilename);
            }
          }

          // add object to link node
          linkNode->addChild(cylinderNode);
        }
    }

    void addBox (const std::string &robotName,
                 const std::string& meshDataRootDir,
		 boost::shared_ptr < urdf::Link >& urdfLink,
		 std::size_t j,
		 GroupNodePtr_t &linkNode, bool visual, bool linkFrame)
    {
      std::string link_name;
      ::boost::shared_ptr< ::urdf::Box > box_shared_ptr;

      if (visual) {
	box_shared_ptr = boost::static_pointer_cast< ::urdf::Box >
	  ( urdfLink->visual_array [j]->geometry);
      } else {
	box_shared_ptr = boost::static_pointer_cast< ::urdf::Box >
	  ( urdfLink->collision_array [j]->geometry);
      }
      link_name = urdfLink->name;
      std::cout << "Box" << std::endl;
      if ( box_shared_ptr != 0 ) {
	std::ostringstream oss;
	oss << robotName << "/" << link_name << "_" << j;
	LeafNodeBoxPtr_t boxNode = LeafNodeBox::create
	  (oss.str (),
	   osgVector3((float)(.5*box_shared_ptr->dim.x),
		      (float)(.5*box_shared_ptr->dim.y), 
		      (float)(.5*box_shared_ptr->dim.z)));
          osgVector3 static_pos; osgQuat static_quat;
	  if (linkFrame) {
	    getStaticTransform (urdfLink, static_pos, static_quat, visual,j);
	  }
          boxNode->setStaticTransform(static_pos,static_quat);

          // Set Color if specified
          if (visual && urdfLink->visual_array [j]->material != NULL) {
            osgVector4 color(urdfLink->visual_array [j]->material->color.r, urdfLink->visual_array [j]->material->color.g, urdfLink->visual_array [j]->material->color.b, urdfLink->visual_array [j]->material->color.a);
            boxNode->setColor(color);
            if (urdfLink->visual_array [j]->material->texture_filename != "") {
              std::string textureFilename = getFilename
                (urdfLink->visual_array [j]->material->texture_filename, meshDataRootDir);
              boxNode->setTexture(textureFilename);
            }
          }
          // add object to link node
          linkNode->addChild(boxNode);
        }
    }

    void addSphere (const std::string &robotName,
                    const std::string& meshDataRootDir,
		    boost::shared_ptr < urdf::Link >& urdfLink,
		    std::size_t j,
		    GroupNodePtr_t &linkNode, bool visual, bool linkFrame)
    {
      std::string link_name;
      ::boost::shared_ptr< ::urdf::Sphere > sphere_shared_ptr;

      if (visual) {
	sphere_shared_ptr = boost::static_pointer_cast < ::urdf::Sphere >
	  ( urdfLink->visual_array [j]->geometry );
      } else {
	sphere_shared_ptr = boost::static_pointer_cast < ::urdf::Sphere >
	  ( urdfLink->collision_array [j]->geometry );
      }
      link_name = urdfLink->name;
      std::cout << "Sphere" << std::endl;
      if ( sphere_shared_ptr != 0 )
        {
	  std::ostringstream oss;
	  oss << robotName << "/" << link_name << "_" << j;
          LeafNodeSpherePtr_t sphereNode
	    (LeafNodeSphere::create(oss.str (),
				    (float)sphere_shared_ptr.get()->radius));
          osgVector3 static_pos; osgQuat static_quat;
	  if (linkFrame) {
	    getStaticTransform (urdfLink, static_pos, static_quat, visual,j);
	  }
          sphereNode->setStaticTransform(static_pos,static_quat);

          // Set Color if specified
          if (visual && urdfLink->visual_array [j]->material != NULL) {
            osgVector4 color(urdfLink->visual_array [j]->material->color.r, urdfLink->visual_array [j]->material->color.g, urdfLink->visual_array [j]->material->color.b, urdfLink->visual_array [j]->material->color.a);
            sphereNode->setColor(color);
            if (urdfLink->visual_array [j]->material->texture_filename != "") {
              std::string textureFilename = getFilename
                (urdfLink->visual_array [j]->material->texture_filename, meshDataRootDir);
              sphereNode->setTexture(textureFilename);
            }
          }

          // add links to link node
          linkNode->addChild(sphereNode);
        }
    }

  }

  GroupNodePtr_t urdfParser::parse (const std::string& robotName,
				    const std::string& urdf_file_path,
				    const std::string& meshDataRootDir,
				    const std::string& collisionOrVisual,
				    const std::string& linkOrObjectFrame)
  {
    if (collisionOrVisual != "visual" && collisionOrVisual != "collision") {
      throw std::runtime_error ("parameter collisionOrVisual should be either "
				"\"collision\" or \"visual\"");
    }
    if (linkOrObjectFrame != "link" && linkOrObjectFrame != "object") {
      throw std::runtime_error ("parameter linkOrObjectFrame should be either "
				"\"link\" or \"object\"");
    }
    bool linkFrame = true;
    if (linkOrObjectFrame == "object") linkFrame = false;

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
      GroupNodePtr_t linkNode (GroupNode::create (robotName + "/" + link_name));
      // add link to robot node
      robot->addChild (linkNode);

      if (collisionOrVisual == "visual") {
	for (std::size_t j = 0; j < links[i]->visual_array.size (); ++j) {
	  switch (links[i]->visual_array [j]->geometry->type) {
	  case urdf::Geometry::MESH:
	    internal_urdf_parser::addMesh (robotName, meshDataRootDir,
					   links [i], j, linkNode, true,
					   linkFrame);
	    break;
	  case urdf::Geometry::CYLINDER:
	    internal_urdf_parser::addCylinder (robotName, meshDataRootDir,
                                               links [i], j, linkNode, true,
                                               linkFrame);
            break;
	  case urdf::Geometry::BOX:
	    internal_urdf_parser::addBox (robotName, meshDataRootDir,
                                          links [i], j, linkNode,
					  true, linkFrame);
	    break;
	  case urdf::Geometry::SPHERE:
	    internal_urdf_parser::addSphere (robotName, meshDataRootDir,
                                             links [i], j, linkNode, true,
                                             linkFrame);
	    break;
	  }
	}
      } else {
	for (std::size_t j=0; j < links[i]->collision_array.size (); ++j) {
	  switch (links[i]->collision_array [j]->geometry->type) {
	  case urdf::Geometry::MESH:
	    internal_urdf_parser::addMesh (robotName, meshDataRootDir,
					   links [i], j, linkNode, false,
					   linkFrame);
	    break;
	  case urdf::Geometry::CYLINDER:
	    internal_urdf_parser::addCylinder (robotName, meshDataRootDir,
                                               links [i], j, linkNode, false,
                                               linkFrame);
	    break;
	  case urdf::Geometry::BOX:
	    internal_urdf_parser::addBox (robotName, meshDataRootDir,
                                          links [i], j, linkNode, false,
                                          linkFrame);
	    break;
	  case urdf::Geometry::SPHERE:
	    internal_urdf_parser::addSphere (robotName, meshDataRootDir,
                                             links [i], j, linkNode, false,
                                             linkFrame);
	    break;
	  }
	}
      }
    }
    return robot;
  }
}
