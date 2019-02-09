// Copyright (c) 2015, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of gepetto-viewer.
// gepetto-viewer is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// gepetto-viewer is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer. If not, see <http://www.gnu.org/licenses/>.

#include "gepetto/viewer/windows-manager.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>

#include <OpenThreads/ScopedLock>

#include <osgDB/WriteFile>

#include <gepetto/viewer/window-manager.h>
#include <gepetto/viewer/node.h>
#include <gepetto/viewer/group-node.h>
#include <gepetto/viewer/leaf-node-box.h>
#include <gepetto/viewer/leaf-node-capsule.h>
#include <gepetto/viewer/leaf-node-cone.h>
#include <gepetto/viewer/leaf-node-cylinder.h>
#include <gepetto/viewer/leaf-node-line.h>
#include <gepetto/viewer/leaf-node-face.h>
#include <gepetto/viewer/leaf-node-sphere.h>
#include <gepetto/viewer/leaf-node-arrow.h>
#include <gepetto/viewer/leaf-node-light.h>
#include <gepetto/viewer/leaf-node-xyzaxis.h>
#include <gepetto/viewer/node-rod.h>
#include <gepetto/viewer/roadmap-viewer.h>
#include <gepetto/viewer/macros.h>
#include <gepetto/viewer/config-osg.h>
#include <gepetto/viewer/leaf-node-ground.h>
#include <gepetto/viewer/leaf-node-collada.h>
#include <gepetto/viewer/urdf-parser.h>
#include <gepetto/viewer/blender-geom-writer.h>

#define RETURN_FALSE_IF_NODE_EXISTS(name)                                      \
  if (nodeExists(name)) {                                                      \
    std::cerr << "Node \"" << name << "\" already exists." << std::endl;       \
    return false;                                                              \
  }

#define RETURN_FALSE_IF_NODE_DOES_NOT_EXIST(name)                              \
  if (!nodeExists(name)) {                                                     \
    std::cerr << "Node \"" << name << "\" does not exist." << std::endl;       \
    return false;                                                              \
  }

#define THROW_IF_NODE_EXISTS(name)                                             \
  if (nodeExists(name)) {                                                      \
    std::ostringstream oss;                                                    \
    oss << "Node \"" << name << "\" already exists.";                          \
    throw std::invalid_argument (oss.str ().c_str ());				       \
  }

#define THROW_IF_NODE_DOES_NOT_EXIST(name)                                     \
  if (!nodeExists(name)) {                                                     \
    std::ostringstream oss;                                                    \
    oss << "Node \"" << name << "\" does not exist.";                          \
    throw std::invalid_argument (oss.str ().c_str ());				       \
  }

#define FIND_NODE_OF_TYPE_OR_THROW(NodeType, varname, nodename)                \
  NodeType##Ptr_t varname (dynamic_pointer_cast <NodeType>                     \
      (getNode(nodename, true)));                                              \
  if (!varname) {                                                              \
    std::ostringstream oss;                                                    \
    oss << "Node \"" << nodename << "\" is not of type " #NodeType;             \
    throw std::invalid_argument (oss.str ().c_str ());				       \
  }

namespace graphics {
    namespace {
      typedef std::map<std::string, NodePtr_t>::iterator          NodeMapIt;
      typedef std::map<std::string, NodePtr_t>::const_iterator    NodeMapConstIt;

      typedef std::map<std::string, GroupNodePtr_t>::iterator          GroupNodeMapIt;
      typedef std::map<std::string, GroupNodePtr_t>::const_iterator    GroupNodeMapConstIt;

      typedef ScopedLock ScopedLock;
    }

    BlenderFrameCapture::BlenderFrameCapture () :
      writer_visitor_ (new TransformWriterVisitor
          (new YamlTransformWriter ("gepetto_viewer.yaml"))),
      nodes_ ()
    {}

    void BlenderFrameCapture::captureFrame ()
    {
      using std::invalid_argument;
      if (!writer_visitor_)
        throw invalid_argument ("Capture writer not defined");
      if (nodes_.empty()) throw invalid_argument ("No node to capture");
      writer_visitor_->captureFrame (nodes_.begin(), nodes_.end());
    }

    WindowsManager::WindowsManager () :
        windowManagers_ (), nodes_ (), groupNodes_ (),roadmapNodes_(),
        osgFrameMtx_ (), configListMtx_ (), newNodeConfigurations_ (),
        autoCaptureTransform_ (false)
    {}

    WindowsManager::WindowID WindowsManager::addWindow (std::string winName,
            WindowManagerPtr_t newWindow)
    {
      WindowID windowId = (WindowID) windowManagers_.size ();
      windowIDmap_ [winName] = windowId;
      windowManagers_.push_back (newWindow);
      addGroup (winName, newWindow->getScene(), false);
      return windowId;
    }

    WindowsManagerPtr_t WindowsManager::create ()
    {
      WindowsManagerPtr_t shPtr (new WindowsManager());
      return shPtr;
    }

    osgVector4 WindowsManager::getColor (const std::string& colorName)
    {
        if (colorName == "blue")
            return osgVector4 (0.f, 0.f, 1.f, 1.f);
        else if (colorName == "green")
            return osgVector4 (0.f, 1.f, 0.f, 1.f);
        else if (colorName == "red")
            return osgVector4 (1.f, 0.f, 0.f, 1.f);
        else if (colorName == "white")
            return osgVector4 (1.f, 1.f, 1.f, 1.f);
        else
            return osgVector4 (0.f, 0.f, 0.f, 1.f);
    }

    VisibilityMode WindowsManager::getVisibility (const std::string& vName)
    {
        if (vName == "OFF")
            return VISIBILITY_OFF;
        else if (vName == "ALWAYS_ON_TOP")
            return ALWAYS_ON_TOP;
        else if (vName == "ON")
            return VISIBILITY_ON;
        else {
            std::cout << "Visibility mode not known, visibility mode can be"
                " \"ON\",\"OFF\" or \"ALWAYS_ON_TOP\"." << std::endl;
            std::cout << "Visibility mode set to default = \"ON\"." << std::endl;
            return VISIBILITY_ON;
        }
    }

    WireFrameMode WindowsManager::getWire (const std::string& wireName)
    {
        if (wireName == "WIREFRAME")
            return WIREFRAME;
        else if (wireName == "FILL_AND_WIREFRAME")
            return FILL_AND_WIREFRAME;
        else if (wireName == "FILL")
            return FILL;
        else {
            std::cout << "Wire mode not known, wire mode can be \"FILL\","
                "\"WIREFRAME\" or \"FILL_AND_WIREFRAME\"." << std::endl;
            std::cout << "Wire mode set to default = \"FILL\"." << std::endl;
            return FILL;
        }
    }

    LightingMode WindowsManager::getLight (const std::string& lightName)
    {
        if (lightName == "OFF")
            return LIGHT_INFLUENCE_OFF;
        else if (lightName == "ON")
            return LIGHT_INFLUENCE_ON;
        else {
            std::cout << "Lighting mode not known, lighting mode can be "
                "\"ON\" or \"OFF\"." << std::endl;
            std::cout << "Lighting mode set to default = \"ON\"." << std::endl;
            return LIGHT_INFLUENCE_ON;
        }
    }

    std::string WindowsManager::parentName (const std::string& name)
    {
        std::string::size_type slash = name.find_last_of ('/');
        if (slash == std::string::npos) return "";
        else return name.substr(0, slash);
    }

    NodePtr_t WindowsManager::find (const std::string name, GroupNodePtr_t)
    {
      NodeMapIt it = nodes_.find (name);
      if (it == nodes_.end ()) {
        std::string::size_type slash = name.find_first_of ('/');
        if (slash == std::string::npos)
          return NodePtr_t ();
        GroupNodeMapIt itg = groupNodes_.find (name.substr (0, slash));
        if (itg == groupNodes_.end ())
          return NodePtr_t ();
        return find (name.substr (slash + 1), itg->second);
      }
      return it->second;
    }

    bool WindowsManager::nodeExists (const std::string& name)
    {
      NodeMapConstIt it = nodes_.find (name);
      return (it != nodes_.end ());
    }

    NodePtr_t WindowsManager::getNode (const std::string& name,
        bool throwIfDoesntExist) const
    {
      NodeMapConstIt it = nodes_.find (name);
      if (it == nodes_.end ()) {
        if (throwIfDoesntExist) {
          std::ostringstream oss;
          oss << "No node with name \"" << name << "\".";
          throw std::invalid_argument (oss.str ().c_str ());
        } else
          return NodePtr_t();
      }
      return it->second;
    }

    template <typename Iterator, typename NodeContainer_t>
      std::size_t WindowsManager::getNodes
      (const Iterator& begin, const Iterator& end, NodeContainer_t& nodes)
    {
      const std::size_t l = nodes.size();
      for (Iterator it = begin; it != end; ++it) {
        std::string name (*it);
        NodePtr_t n = getNode (name);
        if (n) nodes.push_back (n);
        else std::cout << "Node \"" << name << "\" doesn't exist." << std::endl;
      }
      return nodes.size() - l;
    }

    void WindowsManager::initParent (NodePtr_t node, GroupNodePtr_t parent)
    {
      if (parent && !parent->hasChild(node)) parent->addChild (node);
    }

    void WindowsManager::addNode (const std::string& nodeName, NodePtr_t node,
        bool guessParent)
    {
      GroupNodePtr_t parent;
      if (guessParent) parent = getGroup(parentName(nodeName));
      addNode(nodeName, node, parent);
    }

    void WindowsManager::addNode (const std::string& nodeName, NodePtr_t node,
        GroupNodePtr_t parent)
    {
      initParent (node, parent);
      nodes_[nodeName] = node;
    }

    void WindowsManager::addGroup (const std::string& groupName,
            GroupNodePtr_t group,
            bool guessParent)
    {
      GroupNodePtr_t parent;
      if (guessParent) parent = getGroup(parentName(groupName));
      addGroup (groupName, group, parent);
    }

    void WindowsManager::addGroup(const std::string& groupName,
        GroupNodePtr_t group, GroupNodePtr_t parent)
    {
      initParent (group, parent);
      nodes_[groupName] = group;
      groupNodes_[groupName] = group;
    }

    //Public functions

    WindowsManager::WindowID WindowsManager::getWindowID (const std::string& wn)
    {
        WindowIDMap_t::iterator it = windowIDmap_.find (wn);
        if (it == windowIDmap_.end ())
            throw std::invalid_argument ("There is no windows with that name");
        return it->second;
    }

    void WindowsManager::createScene (const std::string& sceneName)
    {
      createGroup(sceneName);
    }

    void WindowsManager::createSceneWithFloor (const std::string& sceneName)
    {
        createScene(sceneName);
        addFloor((sceneName + "/floor").c_str());
    }

    bool WindowsManager::addSceneToWindow (const std::string& sceneName,
            WindowID windowId)
    {
        GroupNodePtr_t group = getGroup(sceneName, true);
        if (windowId < windowManagers_.size ()) {
            ScopedLock lock(osgFrameMutex());
            windowManagers_[windowId]->addNode (group);
            return true;
        }
        else {
            std::cout << "Window ID " << windowId
                << " doesn't exist." << std::endl;
            return false;
        }
    }

     bool WindowsManager::attachCameraToNode(const std::string& nodeName, const WindowID windowId)
     {
        NodePtr_t node = getNode(nodeName, true);
        if (windowId > windowManagers_.size()) {
    	  std::cout << "Window ID" << windowId << "doesn't exist." << std::endl;
  	  return false;
        }
  	ScopedLock lock(osgFrameMutex());
	windowManagers_[windowId]->attachCameraToNode(node);
	return true;
     }

     bool WindowsManager::detachCamera(const WindowID windowId)
     {
        if (windowId > windowManagers_.size()) {
    	  std::cout << "Window ID " << windowId << " doesn't exist."
  		    << std::endl;
  	  return false;
        }
  	ScopedLock lock(osgFrameMutex());
	windowManagers_[windowId]->detachCamera();
	return true;
     }

    bool WindowsManager::addFloor(const std::string& floorName)
    {
        RETURN_FALSE_IF_NODE_EXISTS(floorName);
        LeafNodeGroundPtr_t floor = LeafNodeGround::create (floorName);
        ScopedLock lock(osgFrameMutex());
        addNode (floorName, floor, true);
        return true;
    }

    bool WindowsManager::addBox (const std::string& boxName,
            const float& boxSize1,
            const float& boxSize2,
            const float& boxSize3,
            const Color_t& color)
    {
        RETURN_FALSE_IF_NODE_EXISTS(boxName);

        LeafNodeBoxPtr_t box = LeafNodeBox::create
          (boxName, osgVector3 (.5f*boxSize1, .5f*boxSize2, .5f*boxSize3), color);
        ScopedLock lock(osgFrameMutex());
        addNode (boxName, box, true);
        return true;
    }

    bool WindowsManager::addCapsule (const std::string& capsuleName,
            const float radius,
            const float height,
            const Color_t& color)
    {
        RETURN_FALSE_IF_NODE_EXISTS(capsuleName);

        LeafNodeCapsulePtr_t capsule = LeafNodeCapsule::create (capsuleName, radius, height, color);
        ScopedLock lock(osgFrameMutex());
        addNode (capsuleName, capsule, true);
        return true;
    }

    bool WindowsManager::addArrow (const std::string& arrowName,
            const float radius,
            const float length,
            const Color_t& color)
    {
        RETURN_FALSE_IF_NODE_EXISTS(arrowName);

        LeafNodeArrowPtr_t arrow = LeafNodeArrow::create (arrowName, color, radius, length);
        ScopedLock lock(osgFrameMutex());
        addNode (arrowName, arrow, true);
        return true;
    }

    bool WindowsManager::addRod (const std::string& rodName,
            const Color_t& color,
            const float radius,
            const float length,
            short maxCapsule)
    {
      RETURN_FALSE_IF_NODE_EXISTS(rodName);

      NodeRodPtr_t rod = NodeRod::create(rodName,color,radius,length,maxCapsule);
      ScopedLock lock(osgFrameMutex());
      addNode (rodName, rod, true);
      for(size_t i = 0 ; i < (size_t) maxCapsule ; i++)
        addNode(rod->getCapsuleName(i),rod->getCapsule(i), false);
      return true;
    }

    bool WindowsManager::resizeCapsule(const std::string& capsuleName, float newHeight)
      throw(std::exception)
    {
        FIND_NODE_OF_TYPE_OR_THROW (LeafNodeCapsule, cap, capsuleName);
        ScopedLock lock(osgFrameMutex());
        cap->setHeight(newHeight);
        return true;
    }

    bool WindowsManager::resizeArrow(const std::string& arrowName ,float newRadius, float newLength)
      throw(std::exception)
    {
        FIND_NODE_OF_TYPE_OR_THROW (LeafNodeArrow, arrow, arrowName);
        ScopedLock lock(osgFrameMutex());
        arrow->resize(newRadius,newLength);
        return true;
    }

    bool WindowsManager::addMesh (const std::string& meshName,
            const std::string& meshPath)
    {
        RETURN_FALSE_IF_NODE_EXISTS(meshName);
        LeafNodeColladaPtr_t mesh;
        try {
          mesh = LeafNodeCollada::create (meshName, meshPath);
        } catch (const std::exception& exc) {
          std::cout << exc.what() << std::endl;
          return false;
        }
        ScopedLock lock(osgFrameMutex());
        addNode (meshName, mesh, true);
        return true;
    }

    bool WindowsManager::addCone (const std::string& coneName,
            const float radius, const float height,
            const Color_t&)
    {
        RETURN_FALSE_IF_NODE_EXISTS(coneName);

        LeafNodeConePtr_t cone = LeafNodeCone::create
          (coneName, radius, height);
        ScopedLock lock(osgFrameMutex());
        addNode (coneName, cone, true);
        return true;
    }

    bool WindowsManager::addCylinder (const std::string& cylinderName,
            const float radius,
            const float height,
            const Color_t& color)
    {
        RETURN_FALSE_IF_NODE_EXISTS(cylinderName);

        LeafNodeCylinderPtr_t cylinder = LeafNodeCylinder::create
          (cylinderName, radius, height, color);
        ScopedLock lock(osgFrameMutex());
        addNode (cylinderName, cylinder, true);
        return true;
    }

    bool WindowsManager::addSphere (const std::string& sphereName,
            const float radius,
            const Color_t& color)
    {
        RETURN_FALSE_IF_NODE_EXISTS(sphereName);

        LeafNodeSpherePtr_t sphere = LeafNodeSphere::create
          (sphereName, radius, color);
        ScopedLock lock(osgFrameMutex());
        addNode (sphereName, sphere, true);
        return true;
    }

    bool WindowsManager::addLight (const std::string& lightName,
            const WindowID wid,
            const float radius,
            const Color_t& color)
    {
        RETURN_FALSE_IF_NODE_EXISTS(lightName);

        WindowManagerPtr_t wm = getWindowManager(wid, true);
        LeafNodeLightPtr_t light = LeafNodeLight::create
          (lightName, radius, color);
        ScopedLock lock(osgFrameMutex());
        addNode (lightName, light, true);
        light->setRoot (wm->getScene ());
        return true;
    }

    bool WindowsManager::addLine (const std::string& lineName,
            const osgVector3& pos1,
            const osgVector3& pos2,
            const Color_t& color)
    {
        RETURN_FALSE_IF_NODE_EXISTS(lineName);

        LeafNodeLinePtr_t line = LeafNodeLine::create (lineName, pos1, pos2, color);
        ScopedLock lock(osgFrameMutex());
        addNode (lineName, line, true);
        return true;
    }

  bool WindowsManager::setLineStartPoint(const std::string& lineName,
                                            const osgVector3& pos1)
  {
    FIND_NODE_OF_TYPE_OR_THROW (LeafNodeLine, line, lineName);
    ScopedLock lock(osgFrameMutex());
    line->setStartPoint(pos1);
    return true;
  }

  bool WindowsManager::setLineEndPoint(const std::string& lineName,
                                            const osgVector3& pos2)
  {
    FIND_NODE_OF_TYPE_OR_THROW (LeafNodeLine, line, lineName);
    ScopedLock lock(osgFrameMutex());
    line->setEndPoint(pos2);
    return true;
  }

  bool WindowsManager::setLineExtremalPoints(const std::string& lineName,
                                             const osgVector3& pos1,
                                             const osgVector3& pos2)
  {
    FIND_NODE_OF_TYPE_OR_THROW (LeafNodeLine, line, lineName);
    ScopedLock lock(osgFrameMutex());
    line->setStartPoint(pos1);
    line->setEndPoint(pos2);
    return true;
  }

    bool WindowsManager::addCurve (const std::string& curveName,
            const Vec3ArrayPtr_t& pos,
            const Color_t& color)
    {
      RETURN_FALSE_IF_NODE_EXISTS(curveName);
      if (pos->size () < 2) {
        throw std::invalid_argument ("Need at least two points");
      }
      LeafNodeLinePtr_t curve = LeafNodeLine::create (curveName, pos, color);
      curve->setMode (GL_LINE_STRIP);
      ScopedLock lock(osgFrameMutex());
      addNode (curveName, curve, true);
      return true;
    }

  bool WindowsManager::setCurvePoints (const std::string& curveName,
                                       const Vec3ArrayPtr_t& pos)
  {
    FIND_NODE_OF_TYPE_OR_THROW (LeafNodeLine, curve, curveName);
    if (pos->size () < 2) {
        throw std::invalid_argument ("Need at least two points");
    }
    ScopedLock lock(osgFrameMutex());
    curve->setPoints(pos);
    return true;
  }

    bool WindowsManager::setCurveMode (const std::string& curveName, const GLenum mode)
    {
        FIND_NODE_OF_TYPE_OR_THROW (LeafNodeLine, curve, curveName);
        ScopedLock lock(osgFrameMutex());
        curve->setMode (mode);
        return true;
    }

    bool WindowsManager::setCurvePointsSubset (const std::string& curveName,
        const int first, const std::size_t count)
    {
        FIND_NODE_OF_TYPE_OR_THROW (LeafNodeLine, curve, curveName);
        ScopedLock lock(osgFrameMutex());
        curve->setPointsSubset (first, count);
        return true;
    }

    bool WindowsManager::setCurveLineWidth (const std::string& curveName, const float& width)
    {
        FIND_NODE_OF_TYPE_OR_THROW (LeafNodeLine, curve, curveName);
        setFloatProperty(curveName, "LineWidth", width);
        return true;
    }

    bool WindowsManager::addTriangleFace (const std::string& faceName,
            const osgVector3& pos1,
            const osgVector3& pos2,
            const osgVector3& pos3,
            const Color_t& color)
    {
      RETURN_FALSE_IF_NODE_EXISTS (faceName);

      LeafNodeFacePtr_t face = LeafNodeFace::create (faceName, pos1, pos2, pos3, color);
      ScopedLock lock(osgFrameMutex());
      addNode (faceName, face, true);
      return true;
    }

    bool WindowsManager::addSquareFace (const std::string& faceName,
            const osgVector3& pos1,
            const osgVector3& pos2,
            const osgVector3& pos3,
            const osgVector3& pos4,
            const Color_t& color)
    {
      RETURN_FALSE_IF_NODE_EXISTS(faceName);

      LeafNodeFacePtr_t face = LeafNodeFace::create
        (faceName, pos1, pos2, pos3, pos4, color);
      ScopedLock lock(osgFrameMutex());
      addNode (faceName, face, true);
      return true;
    }

  bool WindowsManager::setTexture (const std::string& nodeName,
				   const std::string& filename)
  {
    FIND_NODE_OF_TYPE_OR_THROW (LeafNodeFace, faceNode, nodeName);

    if (faceNode->nbVertices () != 4) {
      std::ostringstream oss;
      oss << "Face should have 4 vertices to apply texture. "
	  << nodeName << " has " << faceNode->nbVertices () << ".";
      throw std::runtime_error (oss.str ());
    }
    faceNode->setTexture (filename);
    return true;
  }

    bool WindowsManager::addXYZaxis (const std::string& nodeName,const Color_t& color, float radius, float sizeAxis)
    {
      RETURN_FALSE_IF_NODE_EXISTS (nodeName);

      LeafNodeXYZAxisPtr_t axis = LeafNodeXYZAxis::create
        (nodeName,color,radius,sizeAxis);
      ScopedLock lock(osgFrameMutex());
      addNode (nodeName, axis, true);
      return true;
    }

    bool WindowsManager::createRoadmap(const std::string& roadmapName,const Color_t& colorNode, float radius, float sizeAxis, const Color_t& colorEdge)
    {
      RETURN_FALSE_IF_NODE_EXISTS(roadmapName);

      RoadmapViewerPtr_t rm = RoadmapViewer::create(roadmapName,colorNode,radius,sizeAxis,colorEdge);
      ScopedLock lock(osgFrameMutex());
      addNode (roadmapName, rm, true);
      roadmapNodes_[roadmapName]=rm;
      return true;
    }

    bool WindowsManager::addEdgeToRoadmap(const std::string& nameRoadmap, const osgVector3& posFrom, const osgVector3& posTo)
    {
        if (roadmapNodes_.find (nameRoadmap) == roadmapNodes_.end ()) {
            //no node named nodeName
            std::cout << "No roadmap named \"" << nameRoadmap << "\"" << std::endl;
            return false;
        }
        else {
            RoadmapViewerPtr_t rm_ptr = roadmapNodes_[nameRoadmap];
          //  ScopedLock lock(osgFrameMutex()); mtx is now locked only when required in addEdge
            rm_ptr->addEdge(posFrom,posTo,osgFrameMutex());
            return true;
        }
    }

    bool WindowsManager::addNodeToRoadmap(const std::string& nameRoadmap, const Configuration& conf)
    {
        if (roadmapNodes_.find (nameRoadmap) == roadmapNodes_.end ()) {
            //no node named nodeName
            std::cout << "No roadmap named \"" << nameRoadmap << "\"" << std::endl;
            return false;
        }
        else {
            RoadmapViewerPtr_t rm_ptr = roadmapNodes_[nameRoadmap];
           // ScopedLock lock(osgFrameMutex());
            rm_ptr->addNode(conf.position,conf.quat,osgFrameMutex());
            return true;
        }
    }

    std::vector<std::string> WindowsManager::getNodeList ()
    {
        std::vector<std::string> l;
        for (NodeMapIt it=nodes_.begin (); it!=nodes_.end (); ++it) {
            l.push_back (it->first);
        }
        return l;
    }

    std::vector<std::string> WindowsManager::getGroupNodeList (const std::string& group)
    {
        std::vector<std::string> l;
        GroupNodePtr_t g (getGroup(group));
        if(!g)
            std::cout << "Unexisting group: " << group << std::endl;
        else
        {
            std::cout << "List of Nodes in group :" << group << std::endl;
            l.reserve(g->getNumOfChildren());
            for(std::size_t i = 0; i < g->getNumOfChildren(); ++i)
            {
                NodePtr_t node = g->getChild(i);
                l.push_back(node->getID());
                std::cout << '\t' << node->getID() << std::endl;
            }
        }
        return l;
    }

    std::vector<std::string> WindowsManager::getSceneList ()
    {
        std::vector<std::string> l;
        l.reserve(groupNodes_.size());
        std::cout << "List of GroupNodes :" << std::endl;
        for (GroupNodeMapConstIt it =
                groupNodes_.begin (); it!=groupNodes_.end (); ++it) {
            std::cout << "   " << it->first << std::endl;
            l.push_back (it->first);
        }
        return l;
    }

    std::vector<std::string> WindowsManager::getWindowList ()
    {
        std::vector<std::string> l;
        l.reserve(windowIDmap_.size());
        for (WindowIDMap_t::const_iterator it = windowIDmap_.begin ();
                it!=windowIDmap_.end (); ++it) {
            l.push_back (it->first);
        }
        return l;
    }

    bool WindowsManager::createGroup (const std::string& groupName)
    {
      RETURN_FALSE_IF_NODE_EXISTS(groupName);

      GroupNodePtr_t groupNode = GroupNode::create (groupName);
      ScopedLock lock(osgFrameMutex());
      addGroup (groupName, groupNode, true);
      return true;
    }

    bool WindowsManager::addURDF (const std::string& urdfName,
            const std::string& urdfPath)
    {
      return loadUDRF(urdfName, urdfPath, true, true);
    }

    bool WindowsManager::addURDF (const std::string& urdfName,
            const std::string& urdfPath,
            const std::string& /*urdfPackagePath*/)
    {
      return addURDF(urdfName, urdfPath);
    }

    bool WindowsManager::addUrdfCollision (const std::string& urdfName,
            const std::string& urdfPath)
    {
      return loadUDRF(urdfName, urdfPath, false, true);
    }

    bool WindowsManager::addUrdfCollision (const std::string& urdfName,
            const std::string& urdfPath, const std::string& /*urdfPackagePath*/)
    {
      return addUrdfCollision (urdfName, urdfPath);
    }

    void WindowsManager::addUrdfObjects (const std::string& urdfName,
            const std::string& urdfPath,
            bool visual)
    {
      loadUDRF(urdfName, urdfPath, visual, false);
    }

    void WindowsManager::addUrdfObjects (const std::string& urdfName,
            const std::string& urdfPath,
            const std::string& /*urdfPackagePath*/,
            bool visual)
    {
      return addUrdfObjects (urdfName, urdfPath, visual);
    }

    bool WindowsManager::loadUDRF(const std::string& urdfName,
        const std::string& urdfPath, bool visual, bool linkFrame)
    {
      RETURN_FALSE_IF_NODE_EXISTS(urdfName);

      GroupNodePtr_t urdf =
        urdfParser::parse (urdfName, urdfPath, visual, linkFrame);
      ScopedLock lock(osgFrameMutex());
      addGroup (urdfName, urdf, true);
      NodePtr_t link;
      for (std::size_t i=0; i< urdf->getNumOfChildren (); i++) {
        link = urdf->getChild (i);
        GroupNodePtr_t groupNode (dynamic_pointer_cast
            <GroupNode> (link));
        if (groupNode) {
          addGroup(link->getID(), groupNode, urdf);
          for (std::size_t j=0; j < groupNode->getNumOfChildren (); ++j) {
            NodePtr_t object (groupNode->getChild (j));
            addNode(object->getID (), object, groupNode);
          }
        } else {
          addNode(link->getID(), link, urdf);
        }
      }
      return true;
    }

    bool WindowsManager::addToGroup (const std::string& nodeName,
            const std::string& groupName)
    {
        NodePtr_t node = getNode(nodeName, true);
        GroupNodePtr_t group = getGroup(groupName, true);

        if (group->hasChild(node)) return false;

        ScopedLock lock(osgFrameMutex());// if addChild is called in the same time as osg::frame(), gepetto-viewer crash
        groupNodes_[groupName]->addChild (nodes_[nodeName]);
        return true;
    }

    bool WindowsManager::removeFromGroup (const std::string& nodeName,
            const std::string& groupName)
    {
        if (nodes_.find (nodeName) == nodes_.end () ||
                groupNodes_.find (groupName) == groupNodes_.end ()) {
            std::cout << "Node name \"" << nodeName << "\" and/or groupNode \""
                << groupName << "\" doesn't exist." << std::endl;
            return false;
        }
        else {
            ScopedLock lock(osgFrameMutex());
            groupNodes_[groupName]->removeChild(nodes_[nodeName]);
            return true;
        }
    }

    bool WindowsManager::deleteNode (const std::string& nodeName, bool all)
    {
        NodePtr_t n = getNode (nodeName);
        if (!n) return false;
        else {
            /// Check if it is a group
            GroupNodeMapIt it = groupNodes_.find(nodeName);
            if (it != groupNodes_.end ()) {
              if (all) {
                std::vector <std::string> names(it->second->getNumOfChildren ());
                for (std::size_t i = 0; i < names.size(); ++i)
                  names[i] = it->second->getChild (i)->getID();
                {
                  ScopedLock lock(osgFrameMutex());
                  it->second->removeAllChildren ();
                }
                for (std::size_t i = 0; i < names.size(); ++i)
                  deleteNode (names[i], all);
              }
              {
                ScopedLock lock(osgFrameMutex());
                groupNodes_.erase (nodeName);
              }
            }
            GroupNodeMapConstIt itg;
            ScopedLock lock(osgFrameMutex());
            for (itg = groupNodes_.begin (); itg != groupNodes_.end(); ++itg) {
              if (itg->second && itg->second->hasChild (n))
                itg->second->removeChild(n);
            }
            nodes_.erase (nodeName);
            return true;
        }
    }

    bool WindowsManager::applyConfiguration (const std::string& nodeName,
            const Configuration& configuration)
    {
        NodePtr_t updatedNode = getNode (nodeName, false);
        if (!updatedNode) return false;

        NodeConfiguration newNodeConfiguration;
        newNodeConfiguration.node = updatedNode;
        newNodeConfiguration.position = configuration.position;
        newNodeConfiguration.quat = configuration.quat;

        ScopedLock lock(configListMtx_);
        newNodeConfigurations_.push_back (newNodeConfiguration);
        return true;
    }

    bool WindowsManager::applyConfigurations (const std::vector<std::string>& nodeNames,
            const std::vector<Configuration>& configurations)
    {
        if (nodeNames.size() != configurations.size())
          throw std::invalid_argument ("Number of node names and configurations must be equal.");

        newNodeConfigurations_.reserve (
            newNodeConfigurations_.capacity() + nodeNames.size());

        bool success = true;
        ScopedLock lock(configListMtx_);
        for (std::size_t i = 0; i < nodeNames.size(); ++i) {
          NodePtr_t updatedNode = getNode (nodeNames[i], false);
          if (!updatedNode) {
            success = false;
            continue;
          }

          NodeConfiguration newNodeConfiguration;
          newNodeConfiguration.node = updatedNode;
          newNodeConfiguration.position = configurations[i].position;
          newNodeConfiguration.quat = configurations[i].quat;

          newNodeConfigurations_.push_back (newNodeConfiguration);
        }

        return success;
    }

    bool WindowsManager::addLandmark (const std::string& nodeName,
            float size)
    {
        THROW_IF_NODE_DOES_NOT_EXIST(nodeName);
	ScopedLock lock(osgFrameMutex());
        nodes_[nodeName]->addLandmark (size);
        return true;
    }

    bool WindowsManager::deleteLandmark (const std::string& nodeName)
    {
        THROW_IF_NODE_DOES_NOT_EXIST(nodeName);
	ScopedLock lock(osgFrameMutex());
        nodes_[nodeName]->deleteLandmark ();
        return true;
    }

    Configuration WindowsManager::getStaticTransform (const std::string& nodeName) const
    {
        NodePtr_t node = getNode(nodeName, true);
        return Configuration(node->getStaticPosition (),
                             node->getStaticRotation ());
    }

  bool WindowsManager::setStaticTransform (const std::string& nodeName,
      const Configuration& transform)
  {
    RETURN_FALSE_IF_NODE_DOES_NOT_EXIST(nodeName);
    ScopedLock lock(osgFrameMutex());
    nodes_[nodeName]->setStaticTransform(transform.position,transform.quat);
    return true;
  }

    bool WindowsManager::setVisibility (const std::string& nodeName,
            const std::string& visibilityMode)
    {
        RETURN_FALSE_IF_NODE_DOES_NOT_EXIST(nodeName);
        VisibilityMode visibility =  getVisibility (visibilityMode);
	ScopedLock lock(osgFrameMutex());
        nodes_[nodeName]->setVisibilityMode (visibility);
        return true;
    }

    bool WindowsManager::setScale(const std::string& nodeName, const osgVector3& scale)
    {
        THROW_IF_NODE_DOES_NOT_EXIST(nodeName);
        ScopedLock lock(osgFrameMutex());
        nodes_[nodeName]->setScale(scale);
        return true;
    }

    bool WindowsManager::setScale(const std::string& nodeName, const float& scale)
    {
      return setScale(nodeName, osgVector3(scale, scale, scale));
    }

    bool WindowsManager::setScale(const std::string& nodeName, const int& scalePercentage)
    {
      return setScale (nodeName, (value_type)scalePercentage / 100);
    }

    bool WindowsManager::setAlpha(const std::string& nodeName, const float& alpha)
    {
        RETURN_FALSE_IF_NODE_DOES_NOT_EXIST(nodeName);
  	ScopedLock lock(osgFrameMutex());
        nodes_[nodeName]->setAlpha (alpha);
        return true;
    }

    bool WindowsManager::setAlpha(const std::string& nodeName, const int& alphaPercentage)
    {
      return setAlpha (nodeName, (float)alphaPercentage / 100);
    }

    bool WindowsManager::setColor(const std::string& nodeName, const Color_t& color)
    {
        RETURN_FALSE_IF_NODE_DOES_NOT_EXIST(nodeName);
        osgVector4 vecColor(color[0],color[1],color[2],color[3]);
        ScopedLock lock(osgFrameMutex());
        nodes_[nodeName]->setColor (vecColor);
        return true;
    }

    bool WindowsManager::setWireFrameMode (const std::string& nodeName,
            const std::string& wireFrameMode)
    {
        RETURN_FALSE_IF_NODE_DOES_NOT_EXIST(nodeName);
        WireFrameMode wire = getWire (wireFrameMode);
	ScopedLock lock(osgFrameMutex());
        nodes_[nodeName]->setWireFrameMode (wire);
	return true;
    }

    bool WindowsManager::setLightingMode (const std::string& nodeName,
            const std::string& lightingMode)
    {
        RETURN_FALSE_IF_NODE_DOES_NOT_EXIST(nodeName);
        LightingMode light = getLight (lightingMode);
	ScopedLock lock(osgFrameMutex());
        nodes_[nodeName]->setLightingMode (light);
        return true;
    }

    bool WindowsManager::setHighlight (const std::string& nodeName,
            int state)
    {
        RETURN_FALSE_IF_NODE_DOES_NOT_EXIST(nodeName);
	ScopedLock lock(osgFrameMutex());
        nodes_[nodeName]->setHighlightState (state);
        return true;
    }

    bool WindowsManager::setCaptureTransform (const std::string& filename,
        const std::vector<std::string>& nodeNames)
    {
        blenderCapture_.nodes_.clear ();
        std::size_t nb = getNodes (nodeNames.begin(), nodeNames.end(),
            blenderCapture_.nodes_);
        blenderCapture_.writer_visitor_->writer_ =
          new YamlTransformWriter (filename);
        return nb == nodeNames.size();
    }

    void WindowsManager::captureTransformOnRefresh (bool autoCapture)
    {
      autoCaptureTransform_ = autoCapture;
    }

    void WindowsManager::captureTransform ()
    {
        // This requires only a read access to the scene.
        // The only requirement is that no node get delete while accessing them.
	//ScopedLock lock(osgFrameMutex());
        blenderCapture_.captureFrame ();
    }

    bool WindowsManager::writeBlenderScript (const std::string& filename,
        const std::vector<std::string>& nodeNames)
    {
      std::vector<NodePtr_t> nodes;
      std::size_t nb = getNodes (nodeNames.begin(), nodeNames.end(), nodes);
      if (nb != nodeNames.size())
        throw std::invalid_argument ("Could not find one of the nodes");
      BlenderGeomWriterVisitor visitor (filename);
      for (std::size_t i = 0; i < nodes.size(); ++i)
        nodes[i]->accept(visitor);
      return true;
    }

    bool WindowsManager::writeNodeFile (const std::string& nodeName,
        const std::string& filename)
    {
        RETURN_FALSE_IF_NODE_DOES_NOT_EXIST(nodeName);
        ScopedLock lock(osgFrameMutex());
        osg::ref_ptr <osgDB::Options> os = new osgDB::Options;
        os->setOptionString ("NoExtras");
        bool ret = osgDB::writeNodeFile (*nodes_[nodeName]->asGroup (),
            std::string (filename), os.get());
        return ret;
    }

    bool WindowsManager::writeWindowFile (const WindowID windowId,
        const std::string& filename)
    {
        if (windowId < windowManagers_.size ()) {
            ScopedLock lock(osgFrameMutex());
            bool ret = windowManagers_[windowId]->writeNodeFile (std::string (filename));
            return ret;
        }
        else {
            std::cout << "Window ID " << windowId
                << " doesn't exist." << std::endl;
            return false;
        }
    }

    WindowManagerPtr_t WindowsManager::getWindowManager (const WindowID wid,
        bool throwIfDoesntExist) const
    {
      if (wid < windowManagers_.size ()) {
        return windowManagers_[wid];
      }
      else {
        std::ostringstream oss;
        oss << "Window ID " << wid << " doesn't exist.";
        if (throwIfDoesntExist)
          throw std::invalid_argument (oss.str ());
        std::cout << oss.str () << std::endl;
        return WindowManagerPtr_t ();
      }
    }

    GroupNodePtr_t WindowsManager::getGroup (const std::string groupName,
        bool throwIfDoesntExist) const
    {
      GroupNodeMapConstIt it = groupNodes_.find (groupName);
        if (it == groupNodes_.end ()) {
          if (throwIfDoesntExist) {
            std::ostringstream oss;
            oss << "No group with name \"" << groupName << "\".";
            throw std::invalid_argument (oss.str ().c_str ());
          } else
            return GroupNodePtr_t ();
        }
        return it->second;
    }

    Configuration WindowsManager::getNodeGlobalTransform(const std::string nodeName) const
    {
        NodePtr_t node = getNode(nodeName, true);
        std::pair<osgVector3, osgQuat> posQuat = node->getGlobalTransform();
        return Configuration(posQuat.first, posQuat.second);
    }

    bool WindowsManager::setBackgroundColor1(const WindowID windowId,const Color_t& color)
    {
      WindowManagerPtr_t wm = getWindowManager(windowId, true);
      ScopedLock lock(osgFrameMutex());
      wm->setBackgroundColor1(color);
      return true;
    }

  bool WindowsManager::setBackgroundColor2(const WindowID windowId,const Color_t& color)
  {
    WindowManagerPtr_t wm = getWindowManager(windowId, true);
    ScopedLock lock(osgFrameMutex());
    wm->setBackgroundColor2(color);
    return true;
  }

  Configuration WindowsManager::getCameraTransform(const WindowID windowId){
    osg::Quat rot;
    osg::Vec3d pos;
    WindowManagerPtr_t wm = getWindowManager(windowId, true);
    ScopedLock lock(osgFrameMutex());
    wm->getCameraTransform(pos,rot);
    return Configuration(pos,rot);
  }

  bool WindowsManager::setCameraTransform(const WindowID windowId,const Configuration& configuration){
    WindowManagerPtr_t wm = getWindowManager(windowId, true);
    ScopedLock lock(osgFrameMutex());
    wm->setCameraTransform(configuration.position,configuration.quat);
    return true;
  }

  std::vector<std::string> WindowsManager::getPropertyNames(const std::string& nodeName) const
  {
    NodePtr_t node = getNode(nodeName, true);
    const PropertyMap_t map = node->properties();
    std::vector<std::string> names;
    names.reserve(map.size());
    for (PropertyMap_t::const_iterator _prop = map.begin(); _prop != map.end(); ++_prop)
      names.push_back(_prop->first);
    return names;
  }

  std::vector<std::string> WindowsManager::getPropertyTypes(const std::string& nodeName) const
  {
    NodePtr_t node = getNode(nodeName, true);
    const PropertyMap_t map = node->properties();
    std::vector<std::string> types;
    types.reserve(map.size());
    for (PropertyMap_t::const_iterator _prop = map.begin(); _prop != map.end(); ++_prop)
      types.push_back(_prop->second->type());
    return types;
  }

  template <typename Property_t>
  Property_t WindowsManager::getProperty(const std::string& nodeName, const std::string& propName) const
  {
    NodePtr_t node = getNode(nodeName, true);
    Property_t value;
    if (!node->getProperty<Property_t>(propName, value)) {
      throw std::invalid_argument ("Could not get the property");
    }
    return value;
  }

  template <typename Property_t>
  void WindowsManager::setProperty(const std::string& nodeName, const std::string& propName, const Property_t& value)
  {
    ScopedLock lock(osgFrameMutex());
    NodePtr_t node = getNode(nodeName, true);
    if (!node->setProperty<Property_t>(propName, value)) {
      throw std::invalid_argument ("Could not set the property");
    }
  }

#define DEFINE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(Type,Name) \
  Type WindowsManager::get ## Name ## Property(const std::string& nodeName, const std::string& propName) const \
  { return getProperty<Type>(nodeName, propName); } \
  void WindowsManager::set ## Name ## Property(const std::string& nodeName, const std::string& propName, const Type& value) \
  { setProperty<Type>(nodeName, propName, value); }

#define INSTANCIATE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(Type) \
  template Type WindowsManager::getProperty<Type>(const std::string&, const std::string&) const; \
  template void WindowsManager::setProperty<Type>(const std::string&, const std::string&, const Type&)

  INSTANCIATE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(std::string);
  INSTANCIATE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(osgVector3);
  INSTANCIATE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(osgVector4);
  INSTANCIATE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(float);
  INSTANCIATE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(bool);
  INSTANCIATE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(int);

  DEFINE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(std::string, String)
  DEFINE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(osgVector3, Vector3)
  DEFINE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(osgVector4, Color)
  DEFINE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(float, Float)
  DEFINE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(bool, Bool)
  DEFINE_WINDOWS_MANAGER_GET_SET_PROPERTY_FOR_TYPE(int, Int)
} // namespace graphics
