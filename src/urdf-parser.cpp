//
//  urdf-parser.cpp
//  gepetto-viewer
//
//  Created by Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/urdf-parser.h>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <sys/stat.h>

#include <urdf_model/model.h>
#include <urdf_parser/urdf_parser.h>

#include <gepetto/viewer/leaf-node-cylinder.h>
#include <gepetto/viewer/leaf-node-box.h>
#include <gepetto/viewer/leaf-node-sphere.h>

namespace graphics {
  namespace urdfParser {

  namespace {
    typedef boost::shared_ptr<urdf::Link> LinkPtr;
    typedef boost::shared_ptr<urdf::Visual   >    VisualPtr;
    typedef boost::shared_ptr<urdf::Collision> CollisionPtr;
    DEF_CLASS_SMART_PTR(LinkNode)

    class LinkNode :  public GroupNode
    {
      private:
        bool currentIsVisual_;
        std::vector<graphics::NodePtr_t> visuals_, collisions_;

      public:
        virtual ~LinkNode() {}

        LinkNode(const std::string& name) : GroupNode(name)
        {
          addProperty(BoolProperty::create("ShowVisual",
                                           BoolProperty::Getter_t(),
                                           BoolProperty::setterFromMemberFunction(this, &LinkNode::showVisual)));
        }

        void showVisual (const bool& visual)
        {
          if (currentIsVisual_ == visual) return;
          VisibilityMode m = (visual ? VISIBILITY_ON : VISIBILITY_OFF);
          for (std::size_t i = 0; i < visuals_.size(); ++i)
            visuals_[i]->setVisibilityMode(m);
          m = (visual ? VISIBILITY_OFF : VISIBILITY_ON);
          for (std::size_t i = 0; i < collisions_.size(); ++i)
            collisions_[i]->setVisibilityMode(m);
          currentIsVisual_ = visual;
        }

        void add (const NodePtr_t& child_ptr, bool vis)
        {
          VisibilityMode m = (currentIsVisual_ == vis ? VISIBILITY_ON : VISIBILITY_OFF);
          addChild(child_ptr);
          if (vis) visuals_.push_back(child_ptr);
          else collisions_.push_back(child_ptr);
          child_ptr->setVisibilityMode(m);
        }

        static LinkNodePtr_t create (const std::string& name)
        {
          LinkNodePtr_t robot (new LinkNode(name));
          robot->initWeakPtr(robot);
          return robot;
        }
    };

    void split(const std::string &s, char delim, std::vector<std::string> &elems) {
      std::stringstream ss(s);
      std::string item;
      while (std::getline(ss, item, delim)) {
        elems.push_back(item);
      }
    }

    std::vector<std::string> rosPackagePath()
    {
      const std::string rosPathVar (std::getenv("ROS_PACKAGE_PATH"));
      std::vector<std::string> rosPaths;
      split(rosPathVar, ':', rosPaths);
      return rosPaths;
    }

    void getStaticTransform (const LinkPtr& link,
			     osgVector3 &static_pos, osgQuat &static_quat,
			     bool visual, long unsigned int index)
    {
      if (visual) {
        boost::shared_ptr<urdf::Visual> visual;
        if (link->visual_array.size()>1)
          visual = link->visual_array[index];
        else
          visual = link->visual;

        // Set staticTransform = transform from link to visual
        static_pos = osgVector3((float)visual->origin.position.x,
            (float)visual->origin.position.y,
            (float)visual->origin.position.z);

        static_quat=osgQuat( (float)visual->origin.rotation.x,
            (float)visual->origin.rotation.y,
            (float)visual->origin.rotation.z,
            (float)visual->origin.rotation.w);
      } else {
        boost::shared_ptr<urdf::Collision> collision;
        if (link->collision_array.size()>1)
          collision = link->collision_array[index];
        else
          collision = link->collision;

        // Set staticTransform = transform from link to collision
        static_pos = osgVector3((float)collision->origin.position.x,
            (float)collision->origin.position.y,
            (float)collision->origin.position.z);

        static_quat=osgQuat( (float)collision->origin.rotation.x,
            (float)collision->origin.rotation.y,
            (float)collision->origin.rotation.z,
            (float)collision->origin.rotation.w);
      }
    }

    void addMesh (const std::string &robotName,
		  LinkPtr& urdfLink,
		  std::size_t j,
		  LinkNodePtr_t &linkNode, bool visual, bool linkFrame)
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
          mesh_path = getFilename (mesh_shared_ptr->filename);
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

          linkNode->add(meshNode, visual);
          // Set Color if specified
          if (visual && urdfLink->visual_array [j]->material != NULL) {
            osgVector4 color(urdfLink->visual_array [j]->material->color.r, 
			     urdfLink->visual_array [j]->material->color.g, 
			     urdfLink->visual_array [j]->material->color.b, 
			     urdfLink->visual_array [j]->material->color.a);
            meshNode->setColor(color);
            if (urdfLink->visual_array [j]->material->texture_filename != "") {
              std::string textureFilename = getFilename
                (urdfLink->visual_array [j]->material->texture_filename);
              meshNode->setTexture(textureFilename);
            }
          }
        }
    }

    void addCylinder (const std::string &robotName,
		      LinkPtr& urdfLink,
		      std::size_t j,
		      LinkNodePtr_t &linkNode, bool visual, bool linkFrame)
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
                (urdfLink->visual_array [j]->material->texture_filename);
              cylinderNode->setTexture(textureFilename);
            }
          }

          // add object to link node
          linkNode->add(cylinderNode, visual);
        }
    }

    void addBox (const std::string &robotName,
		 LinkPtr& urdfLink,
		 std::size_t j,
		 LinkNodePtr_t &linkNode, bool visual, bool linkFrame)
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
                (urdfLink->visual_array [j]->material->texture_filename);
              boxNode->setTexture(textureFilename);
            }
          }
          // add object to link node
          linkNode->add(boxNode, visual);
        }
    }

    void addSphere (const std::string &robotName,
		    LinkPtr& urdfLink,
		    std::size_t j,
		    LinkNodePtr_t &linkNode, bool visual, bool linkFrame)
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
                (urdfLink->visual_array [j]->material->texture_filename);
              sphereNode->setTexture(textureFilename);
            }
          }

          // add links to link node
          linkNode->add(sphereNode, visual);
        }
    }

    template <typename T> inline const std::vector<T>& getGeomArray (const LinkPtr& link);
    template <> inline const std::vector<   VisualPtr>& getGeomArray<   VisualPtr> (const LinkPtr& link) { return link->   visual_array; }
    template <> inline const std::vector<CollisionPtr>& getGeomArray<CollisionPtr> (const LinkPtr& link) { return link->collision_array; }
    template <bool visual> struct GeomArray { typedef    VisualPtr type; };
    template <> struct GeomArray <false>    { typedef CollisionPtr type; };

    template <bool visual> void addGeoms(const std::string &robotName,
		    LinkPtr& link,
		    LinkNodePtr_t &linkNode, bool linkFrame)
    {
      typedef typename GeomArray<visual>::type BodyType;
      const std::vector<BodyType>& array = getGeomArray<BodyType>(link);
      for (std::size_t j = 0; j < array.size (); ++j) {
        switch (array [j]->geometry->type) {
          case urdf::Geometry::MESH:
            addMesh     (robotName, link, j, linkNode, visual, linkFrame);
            break;
          case urdf::Geometry::CYLINDER:
            addCylinder (robotName, link, j, linkNode, visual, linkFrame);
            break;
          case urdf::Geometry::BOX:
            addBox      (robotName, link, j, linkNode, visual, linkFrame);
            break;
          case urdf::Geometry::SPHERE:
            addSphere   (robotName, link, j, linkNode, visual, linkFrame);
            break;
        }
      }
    }
  }

  std::string getFilename (const std::string& input)
  {
    if (input.substr(0, 10) == "package://") {
      std::vector<std::string> rosPaths = rosPackagePath();
      struct stat sb;
      for (std::size_t i = 0; i < rosPaths.size(); ++i) {
        const std::string name =
          rosPaths[i] + '/' + input.substr(10, std::string::npos); 
        if (stat(name.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
          return name;
      }
      std::cerr << "File not found: " << input << std::endl;
      std::cerr << "Check ROS_PACKAGE_PATH environment variable." << std::endl;
    }
    return input;
  }

  GroupNodePtr_t parse (const std::string& robotName,
			const std::string& urdf_file_path,
			const bool& visual,
			const bool& linkFrame)
  {
    boost::shared_ptr< urdf::ModelInterface > model =
      urdf::parseURDFFile( getFilename(urdf_file_path) );
    // Test that file has correctly been parsed
    if (!model) {
      throw std::runtime_error (std::string ("Failed to parse ") +
				urdf_file_path);
    }
    GroupNodePtr_t robot = GroupNode::create(robotName);
    std::vector< LinkPtr > links;
    model->getLinks(links);
    std::string link_name;

    for (unsigned int i = 0 ; i < links.size() ; i++) {
      link_name = links[i]->name;
      std::cout << link_name << std::endl;
      LinkNodePtr_t linkNode (LinkNode::create (robotName + "/" + link_name));
      linkNode->showVisual(visual);
      // add link to robot node
      robot->addChild (linkNode);

      if (linkFrame ||  visual) addGeoms<true >(robotName, links[i], linkNode, linkFrame);
      if (linkFrame || !visual) addGeoms<false>(robotName, links[i], linkNode, linkFrame);
    }
    return robot;
  }
  } // namespace urdfParser
}
