//
//  urdf-parser.cpp
//  gepetto-viewer
//
//  Created by Anthony Couret, Mathieu Geisert in November 2014.
//  Updated by Joseph Mirabel in February 2019.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/urdf-parser.h>

#include <iostream>
#include <string>
#include <vector>

#include <QBuffer>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QtGlobal>

#include <osgQt/Version>

#include <gepetto/viewer/leaf-node-cylinder.h>
#include <gepetto/viewer/leaf-node-box.h>
#include <gepetto/viewer/leaf-node-sphere.h>


#define OUT(x) std::cout << x << std::endl
// #define OUT(x) (void)0

namespace graphics {
  namespace urdfParser {

  namespace details {
    typedef std::map<std::string, ::osg::NodeRefPtr> Cache_t;
    struct Material {
      bool hasColor, hasTexture;
      osgVector4 color;
      std::string texture;
      Material () : hasColor (false), hasTexture (false) {}
    };
    typedef QMap<QString, Material> MaterialMap_t;
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
                                           BoolProperty::getterFromMemberFunction(this, &LinkNode::showVisual),
                                           BoolProperty::setterFromMemberFunction(this, &LinkNode::showVisual)));
        }

        const bool& showVisual () const
        {
          return currentIsVisual_;
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

    bool getShowVisuals (const GroupNode* gn)
    {
      bool value = true;
      for (std::size_t i = 0; i < gn->getNumOfChildren (); ++i)
      {
        NodePtr_t n = gn->getChild(i);
        if (n->hasProperty("ShowVisual")) {
          bool v;
          n->getProperty ("ShowVisual", v);
          value = value && v;
        }
      }
      return value;
    }
    void setShowVisuals (GroupNode* gn, bool visual)
    {
      for (std::size_t i = 0; i < gn->getNumOfChildren (); ++i)
      {
        NodePtr_t n = gn->getChild(i);
        if (n->hasProperty("ShowVisual"))
          n->setProperty ("ShowVisual", visual);
      }
    }

    QStringList rosPackagePath()
    {
      const QString rosPathVar (qgetenv("ROS_PACKAGE_PATH"));
      return rosPathVar.split(':');
    }

    std::string getFilename (const QString& input)
    {
      if (input.startsWith("package://")) {
        QStringList rosPaths = rosPackagePath();
        for (int i = 0; i < rosPaths.size(); ++i) {
          QFileInfo fileInfo (rosPaths[i] + '/' + input.right (input.size()-10));
          if (fileInfo.exists() && fileInfo.isFile())
            return fileInfo.filePath().toStdString();
        }
        throw std::invalid_argument (("File not found: " + input
              + ". Check ROS_PACKAGE_PATH environment variable.").toStdString());
      }
      return input.toStdString();
    }

    template <typename ReturnType>
    void toDoubles (const QString& s, ReturnType& vect)
    {
      QStringList nums = s.split (' ', QString::SkipEmptyParts);
      if (ReturnType::num_components != nums.size())
        throw std::logic_error ("Could not parse " + s.toStdString());
      bool ok;
      for (int i = 0; i < nums.size(); ++i) {
        double d = nums[i].toDouble (&ok);
        if (!ok) throw std::logic_error ("Could not parse " + s.toStdString());
        vect[i] = d;
      }
    }

    template <typename ReturnType>
    void toFloats (const QString& s, ReturnType& vect)
    {
      QStringList nums = s.split (' ', QString::SkipEmptyParts);
      if (ReturnType::num_components != nums.size())
        throw std::logic_error ("Could not parse " + s.toStdString());
      bool ok;
      for (int i = 0; i < nums.size(); ++i) {
        float d = nums[i].toFloat (&ok);
        if (!ok) throw std::logic_error ("Could not parse " + s.toStdString());
        vect[i] = d;
      }
    }

    void parseOrigin (const QDomElement element, osgVector3& T, osgQuat& R)
    {
      QDomElement origin = element.firstChildElement ("origin");
      if (!origin.nextSiblingElement ("origin").isNull())
        throw std::logic_error ("URDF contains two origins.");

      T = osgVector3 (0,0,0);
      R = osgQuat    (0,0,0,1);
      if (origin.isNull()) return;
      QString xyz = origin.attribute ("xyz");
      if (!xyz.isNull())
        toFloats (xyz, T);
      QString rpy = origin.attribute ("rpy");
      if (!rpy.isNull()) {
        osgVector3 rpy2;
        toFloats (rpy, rpy2);

        double phi, the, psi;
        phi = rpy2[0] / 2.0;
        the = rpy2[1] / 2.0;
        psi = rpy2[2] / 2.0;

        R.x() = sin(phi) * cos(the) * cos(psi) - cos(phi) * sin(the) * sin(psi);
        R.y() = cos(phi) * sin(the) * cos(psi) + sin(phi) * cos(the) * sin(psi);
        R.z() = cos(phi) * cos(the) * sin(psi) - sin(phi) * sin(the) * cos(psi);
        R.w() = cos(phi) * cos(the) * cos(psi) + sin(phi) * sin(the) * sin(psi);
      }
    }

    NodePtr_t createMesh (const QString& name,
		  const QDomElement mesh,
                  Cache_t& cache)
    {
      std::string mesh_path = getFilename (mesh.attribute("filename"));

      Cache_t::const_iterator _cache = cache.find (mesh_path);
      LeafNodeColladaPtr_t meshNode;
      if (_cache == cache.end()) {
        meshNode = LeafNodeCollada::create (name.toStdString(), mesh_path);
        cache.insert (std::make_pair(mesh_path, meshNode->getColladaPtr()));
      } else
        meshNode = LeafNodeCollada::create (name.toStdString(), _cache->second, mesh_path);

      if (mesh.hasAttribute ("scale")) {
        osgVector3 scale (1,1,1);
        toFloats (mesh.attribute("scale"), scale);
        meshNode->setScale(scale);
      }

      return meshNode;
    }

    Material parseMaterial (const QDomElement material)
    {
      Material mat;

      // Set color
      QDomElement color = material.firstChildElement ("color");
      mat.hasColor = !color.isNull();
      if (mat.hasColor) {
        toFloats (color.attribute ("rgba"), mat.color);
      }

      // Set texture
      QDomElement texture = material.firstChildElement ("texture");
      mat.hasTexture = !texture.isNull();
      if (mat.hasTexture) {
        if (!texture.hasAttribute ("filename"))
          throw std::logic_error ("texture tag must have a filename attribute.");
        mat.texture = getFilename (texture.attribute("filename"));
      }
      return mat;
    }

    void setMaterial (const QDomElement material,
        NodePtr_t node,
        const MaterialMap_t& materials)
    {
      if (material.isNull()) return;
      Material mat;

      if (!material.hasAttribute ("name"))
        throw std::logic_error ("material tag must have a name attribute.");

      QString name = material.attribute ("name");
      if (materials.contains (name)) {
        mat = materials[name];
      } else {
        // Parse material
        mat = parseMaterial (material);
      }

      // Set color
      if (mat.hasColor)
        node->setColor (mat.color);

      // Set texture
      if (mat.hasTexture) {
        NodeDrawablePtr_t    nd = dynamic_pointer_cast<NodeDrawable   > (node);
        LeafNodeColladaPtr_t lc = dynamic_pointer_cast<LeafNodeCollada> (node);
        assert (nd || lc);
        if      (nd) nd->setTexture(mat.texture);
        else if (lc) lc->setTexture(mat.texture);
      }
    }

    template <bool visual> void addGeoms(const std::string &robotName,
                    const QString& namePrefix,
		    const QDomElement link,
		    LinkNodePtr_t &linkNode, bool linkFrame,
                    Cache_t& cache,
                    const MaterialMap_t& materials)
    {
      static const QString tagName (visual ? "visual" : "collision");
      static const QString nameFormat ("%1/%2_%3");
      QString name = nameFormat
        .arg (robotName.c_str())
        .arg (namePrefix);

      int index = 0;
      for (QDomElement element = link.firstChildElement (tagName);
          !element.isNull();
          element = element.nextSiblingElement (tagName)) {
        NodePtr_t node;
        QString name_i = name.arg (index);

        // Parse tag geometry
        QDomElement geometry = element.firstChildElement("geometry");
        if (!geometry.nextSiblingElement ("geometry").isNull())
          throw std::logic_error ("Visual or collision tag contains two geometries.");
        int N = 0;
        bool ok;
        for (QDomElement type = geometry.firstChildElement ();
            !type.isNull(); type = type.nextSiblingElement ()) {
          if        (type.tagName() == "box") {
            // Create box
            osgVector3 sideLengths;
            toFloats (type.attribute("size"), sideLengths);
            node = LeafNodeBox::create (name_i.toStdString (), sideLengths * .5f);
            ++N;
          } else if (type.tagName() == "cylinder") {
            float radius = type.attribute("radius").toFloat (&ok);
            if (!ok) throw std::logic_error ("Could not parse cylinder radius.");
            float length = type.attribute("length").toFloat (&ok);
            if (!ok) throw std::logic_error ("Could not parse cylinder length.");
	    node = LeafNodeCylinder::create (name_i.toStdString(), radius, length);
            ++N;
          } else if (type.tagName() == "sphere") {
            float radius = type.attribute("radius").toFloat (&ok);
            if (!ok) throw std::logic_error ("Could not parse sphere radius.");
            node = LeafNodeSphere::create(name_i.toStdString(), radius);
            ++N;
          } else if (type.tagName() == "mesh") {
            node = createMesh (name_i, type, cache);
            ++N;
          }
        }
        if (N != 1)
          throw std::logic_error ("geometry tag must contain only one of box, cylinder, sphere or mesh.");

        // Parse tag origin
        if (linkFrame) {
          osgVector3 static_pos; osgQuat static_quat;
          parseOrigin (element, static_pos, static_quat);
          node->setStaticTransform(static_pos,static_quat);
        }

        // Parse tag meterial
        setMaterial (element.firstChildElement ("material"),
            node, materials);

        linkNode->add(node, visual);
        ++index;
      }
    }

    void parseXML (const std::string& urdf_file, QDomDocument& model)
    {
      bool ok;
      QString errorPrefix, errorMsg;
      int errorRow, errorCol;

      if (urdf_file.compare(urdf_file.length() - 5, 5, ".urdf") == 0) {
        QFile file (urdf_file.c_str());
        ok = model.setContent (&file, false, &errorMsg, &errorRow, &errorCol);
        errorPrefix = "Failed to parse ";
        errorPrefix +=  urdf_file.c_str();
      } else {
        QBuffer buffer;
        buffer.setData (urdf_file.c_str(), (int)urdf_file.length());
        ok = model.setContent (&buffer, false, &errorMsg, &errorRow, &errorCol);
        errorPrefix = "Failed to parse XML string";
      }

      if (!ok) {
        throw std::invalid_argument ( QString ("%1: %2 at %3:%4")
            .arg (errorPrefix)
            .arg (errorMsg)
            .arg (errorRow)
            .arg (errorCol)
            .toStdString()
            );
      }
    }
  }

  std::string getFilename (const std::string& input)
  {
    return details::getFilename (QString::fromStdString(input));
  }

  GroupNodePtr_t parse (const std::string& robotName,
			const std::string& urdf_file,
			const bool& visual,
			const bool& linkFrame)
  {
    QDomDocument model;

    // Parse the XML document
    details::parseXML (urdf_file, model);

    // Parse the materials
    details::MaterialMap_t materials;
    for (QDomElement material = model.firstChildElement("robot").firstChildElement("material");
        !material.isNull();
        material = material.nextSiblingElement ("material")) {
      if (!material.hasAttribute ("name"))
        throw std::logic_error ("material tag must have a name attribute.");

      details::Material mat = details::parseMaterial (material);

      if (!mat.hasColor && !mat.hasTexture)
        throw std::logic_error ("material tag must have either a color or a texture.");
      QString name = material.attribute ("name");
      materials[name] = mat;
    }
        

    GroupNodePtr_t robot = GroupNode::create(robotName);
    QDomNodeList links = model.elementsByTagName ("link");

    robot->addProperty (
        BoolProperty::create("ShowVisual",
          BoolProperty::Getter_t(boost::bind(details::getShowVisuals, robot.get())),
          BoolProperty::Setter_t(boost::bind(details::setShowVisuals, robot.get(), _1))
          ));

    details::Cache_t cache;
    for (int i = 0 ; i < links.size() ; i++) {
      QDomElement link = links.at(i).toElement();
      if (link.isNull()) throw std::logic_error ("link must be a tag.");
      QString name = link.attribute ("name");
      if (name.isNull()) throw std::logic_error ("A link has no name attribute.");

      std::string link_name = name.toStdString();
      OUT( link_name );

      details::LinkNodePtr_t linkNode (details::LinkNode
          ::create (robotName + "/" + link_name));
      linkNode->showVisual(visual);
      // add link to robot node
      robot->addChild (linkNode);

      if (visual) {
        details::addGeoms<true >(robotName, name, link, linkNode, linkFrame, cache, materials);
        if (linkFrame) {
          try {
            details::addGeoms<false>(robotName, "collision_"+name, link, linkNode, linkFrame, cache, materials);
          } catch (const std::invalid_argument& e) {
            std::cerr << "Could not load collision geometries of " << robotName << ":"
              << e.what() << std::endl;
          }
        }
      } else {
        details::addGeoms<false >(robotName, name, link, linkNode, linkFrame, cache, materials);
        if (linkFrame) {
          try {
            details::addGeoms<true>(robotName, "visual_"+name, link, linkNode, linkFrame, cache, materials);
          } catch (const std::invalid_argument& e) {
            std::cerr << "Could not load visual geometries of " << robotName << ":"
              << e.what() << std::endl;
          }
        }
      }
    }
    return robot;
  }
  } // namespace urdfParser
}
