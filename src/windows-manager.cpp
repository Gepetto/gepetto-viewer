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

#include <boost/thread.hpp>

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
#include <gepetto/viewer/macros.h>
#include <gepetto/viewer/config-osg.h>
#include <gepetto/viewer/leaf-node-ground.h>
#include <gepetto/viewer/leaf-node-collada.h>
#include <gepetto/viewer/urdf-parser.h>

namespace graphics {
    WindowsManager::WindowsManager () :
        windowManagers_ (), nodes_ (), groupNodes_ (),
        mtx_ (), rate_ (20), newNodeConfigurations_ ()
    {
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

    osgVector4 WindowsManager::getColor (const value_type* colorCorba)
    {
        return osgVector4 (colorCorba[0], colorCorba[1], colorCorba[2],
                colorCorba[3]);
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
        std::string Name (name);
        std::string::iterator parentNameIt;
        for (std::string::iterator i = Name.end (); (*i) != char ('/') &&
                i != Name.begin (); i--) {
            parentNameIt = i;
        }
        parentNameIt--;

        std::string parentName;
        for (std::string::iterator i = Name.begin (); i != parentNameIt; i++) {
            parentName.push_back (*i);
        }

        return parentName;
    }

    void WindowsManager::initParent (const std::string& nodeName,
            NodePtr_t node)
    {
        GroupNodePtr_t groupNode = groupNodes_
            [WindowsManager::parentName (nodeName)];
        if (groupNode) {
            groupNode->addChild (node);
        }
    }

    void WindowsManager::addNode (const std::string& nodeName, NodePtr_t node)
    {
        nodes_[nodeName] = node;
    }

    void WindowsManager::addGroup (const std::string& groupName,
            GroupNodePtr_t group)
    {
        groupNodes_[groupName] = group;
        nodes_[groupName] = group;
    }

    void WindowsManager::threadRefreshing (WindowManagerPtr_t window)
    {
        while (!window->done ())
        {
            mtx_.lock ();
            window->frame ();
            mtx_.unlock ();
            boost::this_thread::sleep (boost::posix_time::milliseconds (rate_));
        }
    }

    osgQuat WindowsManager::corbaConfToOsgQuat (const value_type* configCorba)
    {
        // configurationCorba = trans (x, y, z), quat (w, x, y, z)
        // osgQuat = quat (x, y, z, w)
        return osgQuat (configCorba[4], configCorba[5],
                configCorba[6], configCorba[3]);
    }

    osgVector3 WindowsManager::corbaConfToOsgVec3 (const value_type* configCorba)
    {
        return osgVector3 (configCorba[0], configCorba[1], configCorba[2]);
    }


    //Public functions

    bool WindowsManager::setRate (const int& rate)
    {
        if (rate <= 0) {
            std::cout << "You should specify a positive rate" << std::endl;
            return false;
        }
        else {
            rate_ = rate;
            return true;
        }
    }

    WindowsManager::WindowID WindowsManager::createWindow (const char* winName)
    {
        std::string wn (winName);
        WindowManagerPtr_t newWindow = WindowManager::create ();
        WindowID windowId = windowManagers_.size ();
        windowManagers_.push_back (newWindow);
        boost::thread refreshThread (boost::bind
                (&WindowsManager::threadRefreshing,
                 this, newWindow));
        return windowId;
    }

    void WindowsManager::refresh ()
    {
        mtx_.lock ();
        //refresh scene with the new configuration
        for (std::list<NodeConfiguration>::iterator it =
                newNodeConfigurations_.begin ();
                it != newNodeConfigurations_.end (); it++) {
            (*it).node->applyConfiguration ( (*it).position, (*it).quat);
        }
        for (WindowManagerVector_t::iterator it = windowManagers_.begin ();
                it!=windowManagers_.end (); ++it)
            (*it)->frame ();
        mtx_.unlock ();
        newNodeConfigurations_.clear ();
    }

    void WindowsManager::createScene (const char* sceneNameCorba)
    {
        std::string sceneName (sceneNameCorba);
        if (nodes_.find (sceneName) != nodes_.end ()) {
            std::ostringstream oss;
            oss << "A scene with name, \"" << sceneName << "\" already exists.";
            throw std::runtime_error (oss.str ());
        }
        else {
            GroupNodePtr_t mainNode = GroupNode::create (sceneName);
            addGroup (sceneName, mainNode);
        }
    }

    void WindowsManager::createSceneWithFloor (const char* sceneNameCorba)
    {
        std::string sceneName (sceneNameCorba);
        if (nodes_.find (sceneName) != nodes_.end ()) {
            std::ostringstream oss;
            oss << "A scene with name, \"" << sceneName << "\" already exists.";
            throw std::runtime_error (oss.str ());
        }
        else {
            GroupNodePtr_t mainNode = GroupNode::create (sceneName);
            addGroup (sceneName, mainNode);
            std::string floorName = sceneName + "/floor";
            LeafNodeGroundPtr_t floor = LeafNodeGround::create (floorName);
            addNode (floorName, floor);
            mainNode->addChild (floor);
        }
    }

    bool WindowsManager::addSceneToWindow (const char* sceneNameCorba,
            WindowID windowId)
    {
        std::string sceneName (sceneNameCorba);
        if ((windowId >= 0 || windowId < windowManagers_.size ()) &&
                groupNodes_.find (sceneName) != groupNodes_.end () ) {
            windowManagers_[windowId]->addNode (groupNodes_[sceneName]);
            return true;
        }
        else {
            std::cout << "Window ID \"" << windowId
                << "\" and/or scene name \"" << sceneName
                << "\" doesn't exist." << std::endl;
            return false;
        }
    }

    bool WindowsManager::addBox (const char* boxNameCorba,
            const float boxSize1,
            const float boxSize2,
            const float boxSize3,
            const value_type* colorCorba)
    {
        std::string boxName (boxNameCorba);
        if (nodes_.find (boxName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \"" << boxName
                << "\" already exist." << std::endl;
            return false;
        }
        else {
            LeafNodeBoxPtr_t box = LeafNodeBox::create
                (boxName, osgVector3 (boxSize1, boxSize2, boxSize3),
                 getColor (colorCorba));
            WindowsManager::initParent (boxName, box);
            addNode (boxName, box);
            return true;
        }
    }

    bool WindowsManager::addCapsule (const char* capsuleNameCorba,
            const float radius,
            const float height,
            const value_type* colorCorba)
    {
        const std::string capsuleName (capsuleNameCorba);
        if (nodes_.find (capsuleName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \""
                << capsuleName << "\" already exist." << std::endl;
            return false;
        }
        else {
            LeafNodeCapsulePtr_t capsule = LeafNodeCapsule::create
                (capsuleName, radius, height, getColor (colorCorba));
            WindowsManager::initParent (capsuleName, capsule);
            addNode (capsuleName, capsule);
            return true;
        }
    }

    bool WindowsManager::addMesh (const char* meshNameCorba,
            const char* meshPathCorba)
    {
        std::string meshName (meshNameCorba);
        std::string meshPath (meshPathCorba);
        if (nodes_.find (meshName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \"" << meshName
                << "\" already exist." << std::endl;
            return false;
        }
        else {
            try {
                LeafNodeColladaPtr_t mesh = LeafNodeCollada::create
                    (meshName, meshPath);
                WindowsManager::initParent (meshName, mesh);
                addNode (meshName, mesh);
                return true;
            } catch (const std::exception& exc) {
                std::cout << "Mesh \"" << meshPath << "\" not found." << std::endl;
                return false;
            }
        }
    }

    bool WindowsManager::addCone (const char* coneNameCorba,
            const float radius, const float height,
            const value_type*)
    {
        std::string coneName (coneNameCorba);
        if (nodes_.find (coneName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \"" << coneName
                << "\" already exist." << std::endl;
            return false;
        }
        else {
            LeafNodeConePtr_t cone = LeafNodeCone::create
                (coneName, radius, height);
            WindowsManager::initParent (coneName, cone);
            addNode (coneName, cone);
            return true;
        }
    }

    bool WindowsManager::addCylinder (const char* cylinderNameCorba,
            const float radius,
            const float height,
            const value_type* colorCorba)
    {
        std::string cylinderName (cylinderNameCorba);
        if (nodes_.find (cylinderName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \"" << cylinderName
                << "\" already exist." << std::endl;
            return false;
        }
        else {
            LeafNodeCylinderPtr_t cylinder = LeafNodeCylinder::create
                (cylinderName, radius, height, getColor (colorCorba));
            WindowsManager::initParent (cylinderName, cylinder);
            addNode (cylinderName, cylinder);
            return true;
        }
    }

    bool WindowsManager::addSphere (const char* sphereNameCorba,
            const float radius,
            const value_type* colorCorba)
    {
        std::string sphereName (sphereNameCorba);
        if (nodes_.find (sphereName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \"" << sphereName
                << "\" already exist." << std::endl;
            return false;
        }
        else {
            LeafNodeSpherePtr_t sphere = LeafNodeSphere::create
                (sphereName, radius, getColor (colorCorba));
            WindowsManager::initParent (sphereName, sphere);
            addNode (sphereName, sphere);
            return true;
        }
    }

    bool WindowsManager::addLine (const char* lineNameCorba,
            const value_type* posCorba1,
            const value_type* posCorba2,
            const value_type* colorCorba)
    {
        std::string lineName (lineNameCorba);
        if (nodes_.find (lineName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \"" << lineName
                << "\" already exist." << std::endl;
            return false;
        }
        else {
            osgVector3 pos1 (posCorba1[0], posCorba1[1], posCorba1[2]);
            osgVector3 pos2 (posCorba2[0], posCorba2[1], posCorba2[2]);
            LeafNodeLinePtr_t line = LeafNodeLine::create
                (lineName, pos1, pos2, getColor (colorCorba));
            WindowsManager::initParent (lineName, line);
            addNode (lineName, line);
            return true;
        }
    }

    bool WindowsManager::addTriangleFace (const char* faceNameCorba,
            const value_type* posCorba1,
            const value_type* posCorba2,
            const value_type* posCorba3,
            const value_type* colorCorba)
    {
        std::string faceName (faceNameCorba);
        if (nodes_.find (faceName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \"" << faceName
                << "\" already exist." << std::endl;
            return false;
        }
        else {
            osgVector3 pos1 (posCorba1[0], posCorba1[1], posCorba1[2]);
            osgVector3 pos2 (posCorba2[0], posCorba2[1], posCorba2[2]);
            osgVector3 pos3 (posCorba3[0], posCorba3[1], posCorba3[2]);
            LeafNodeFacePtr_t face = LeafNodeFace::create
                (faceName, pos1, pos2, pos3, getColor (colorCorba));
            WindowsManager::initParent (faceName, face);
            addNode (faceName, face);
            return true;
        }
    }

    bool WindowsManager::addSquareFace (const char* faceNameCorba,
            const value_type* posCorba1,
            const value_type* posCorba2,
            const value_type* posCorba3,
            const value_type* posCorba4,
            const value_type* colorCorba)
    {
        std::string faceName (faceNameCorba);
        if (nodes_.find (faceName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \"" << faceName
                << "\" already exist." << std::endl;
            return false;
        }
        else {
            osgVector3 pos1 (posCorba1[0], posCorba1[1], posCorba1[2]);
            osgVector3 pos2 (posCorba2[0], posCorba2[1], posCorba2[2]);
            osgVector3 pos3 (posCorba3[0], posCorba3[1], posCorba3[2]);
            osgVector3 pos4 (posCorba4[0], posCorba4[1], posCorba4[2]);
            LeafNodeFacePtr_t face = LeafNodeFace::create
                (faceName, pos1, pos2, pos3, pos3, getColor (colorCorba));
            WindowsManager::initParent (faceName, face);
            addNode (faceName, face);
            return true;
        }
    }

    void WindowsManager::getNodeList ()
    {
        std::cout << "List of Nodes :" << std::endl;
        for (std::map<std::string, NodePtr_t>::iterator it=nodes_.begin ();
                it!=nodes_.end (); ++it)
            std::cout << "   " << it->first << std::endl;
        std::cout << "List of GroupNodes :" << std::endl;
        for (std::map<std::string, GroupNodePtr_t>::iterator it=
                groupNodes_.begin (); it!=groupNodes_.end (); ++it)
            std::cout << "   " << it->first << std::endl;
    }

    void WindowsManager::getWindowList ()
    {
        std::cout << "List of Windows :" << std::endl;
        size_t rank = 0;
        for (WindowManagerVector_t::iterator it = windowManagers_.begin ();
                it!=windowManagers_.end (); ++it)
            std::cout << rank << " - " << (*it)->getViewerClone ()->getSlave (0)._camera->getGraphicsContext ()->getTraits ()->windowName << std::endl;
    }

    bool WindowsManager::createGroup (const char* groupNameCorba)
    {
        const std::string groupName (groupNameCorba);
        if (nodes_.find (groupName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \"" << groupName
                << "\" already exist." << std::endl;
            return false;
        }
        else {
            GroupNodePtr_t groupNode = GroupNode::create (groupName);
            WindowsManager::initParent (groupName, groupNode);
            addGroup (groupName, groupNode);
            return true;
        }
    }

    bool WindowsManager::addURDF (const char* urdfNameCorba,
            const char* urdfPathCorba,
            const char* urdfPackagePathCorba)
    {
        const std::string urdfName (urdfNameCorba);
        const std::string urdfPath (urdfPathCorba);
        const std::string urdfPackagePath (urdfPackagePathCorba);
        if (nodes_.find (urdfName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \"" << urdfName
                << "\" already exist." << std::endl;
            return false;
        }
        else {
            GroupNodePtr_t urdf = urdfParser::parse
                (urdfName, urdfPath, urdfPackagePath);
            NodePtr_t link;
            for (int i=0; i< urdf->getNumOfChildren (); i++) {
                link = urdf->getChild (i);
                nodes_[link->getID ()] = link;
            }
            WindowsManager::initParent (urdfName, urdf);
            addGroup (urdfName, urdf);
            return true;
        }
    }

    bool WindowsManager::addUrdfCollision (const char* urdfNameCorba,
            const char* urdfPathCorba, const char* urdfPackagePathCorba)
    {
        const std::string urdfName (urdfNameCorba);
        const std::string urdfPath (urdfPathCorba);
        const std::string urdfPackagePath (urdfPackagePathCorba);
        if (nodes_.find (urdfName) != nodes_.end ()) {
            std::cout << "You need to chose an other name, \"" << urdfName
                << "\" already exist." << std::endl;
            return false;
        }
        else {
            GroupNodePtr_t urdf = urdfParser::parse
                (urdfName, urdfPath, urdfPackagePath, "collision");
            NodePtr_t link;
            for (int i=0; i< urdf->getNumOfChildren (); i++) {
                link = urdf->getChild (i);
                nodes_[link->getID ()] = link;
            }
            WindowsManager::initParent (urdfName, urdf);
            addGroup (urdfName, urdf);
            return true;
        }
    }

    void WindowsManager::addUrdfObjects (const char* urdfNameCorba,
            const char* urdfPathCorba,
            const char* urdfPackagePathCorba,
            bool visual)
    {
        const std::string urdfName (urdfNameCorba);
        const std::string urdfPath (urdfPathCorba);
        const std::string urdfPackagePath (urdfPackagePathCorba);
        if (urdfName == "") {
            throw std::runtime_error ("Parameter nodeName cannot be empty in "
                    "idl request addUrdfObjects.");
        }
        if (nodes_.find (urdfName) != nodes_.end ()) {
            std::ostringstream oss;
            oss << "You need to chose an other name, \"" << urdfName
                << "\" already exist.";
            throw std::runtime_error (oss.str ());
        }
        GroupNodePtr_t urdf = urdfParser::parse
            (urdfName, urdfPath, urdfPackagePath,
             visual ? "visual" : "collision", "object");
        NodePtr_t link;
        for (int i=0; i< urdf->getNumOfChildren (); i++) {
            link = urdf->getChild (i);
            nodes_[link->getID ()] = link;
        }
        WindowsManager::initParent (urdfName, urdf);
        addGroup (urdfName, urdf);
    }

    bool WindowsManager::addToGroup (const char* nodeNameCorba,
            const char* groupNameCorba)
    {
        const std::string nodeName (nodeNameCorba);
        const std::string groupName (groupNameCorba);
        if (nodes_.find (nodeName) == nodes_.end () ||
                groupNodes_.find (groupName) == groupNodes_.end ()) {
            std::cout << "Node name \"" << nodeName << "\" and/or groupNode \""
                << groupName << "\" doesn't exist." << std::endl;
            return false;
        }
        else {
            groupNodes_[groupName]->addChild (nodes_[nodeName]);
            return true;
        }
    }

    bool WindowsManager::applyConfiguration (const char* nodeNameCorba,
            const value_type* configurationCorba)
    {
        const std::string nodeName (nodeNameCorba);
        if (nodes_.find (nodeName) == nodes_.end ()) {
            //no node named nodeName
            std::cout << "No Node named \"" << nodeName << "\"" << std::endl;
            return false;
        }
        else {
            NodeConfiguration newNodeConfiguration;
            newNodeConfiguration.node = nodes_[nodeName];
            try {
                newNodeConfiguration.position =
                    WindowsManager::corbaConfToOsgVec3 (configurationCorba);
                newNodeConfiguration.quat =
                    WindowsManager::corbaConfToOsgQuat (configurationCorba);
            } catch (const std::exception& exc) {
                std::cout << "Error when converting configuration, configuration "
                    "should be DOUBLE[7] = [transX, transY, transZ, quatW, quatX,"
                    "quatY, quatZ]" <<std::endl;
                return false;
            }
            newNodeConfigurations_.push_back (newNodeConfiguration);
            return true;
        }
    }

    bool WindowsManager::addLandmark (const char* nodeNameCorba,
            float size)
    {
        const std::string nodeName (nodeNameCorba);
        if (nodes_.find (nodeName) == nodes_.end ()) {
            std::cout << "Node \"" << nodeName << "\" doesn't exist."
                << std::endl;
            return false;
        }
        nodes_[nodeName]->addLandmark (size);
        return true;
    }

    bool WindowsManager::deleteLandmark (const char* nodeNameCorba)
    {
        const std::string nodeName (nodeNameCorba);
        if (nodes_.find (nodeName) == nodes_.end ()) {
            std::cout << "Node \"" << nodeName << "\" doesn't exist."
                << std::endl;
            return false;
        }
        nodes_[nodeName]->deleteLandmark ();
        return true;
    }

    bool WindowsManager::setVisibility (const char* nodeNameCorba,
            const char* visibilityModeCorba)
    {
        const std::string nodeName (nodeNameCorba);
        const std::string visibilityMode (visibilityModeCorba);
        VisibilityMode visibility =  getVisibility (visibilityMode);
        if (nodes_.find (nodeName) == nodes_.end ()) {
            std::cout << "Node \"" << nodeName << "\" doesn't exist."
                << std::endl;
            return false;
        }
        nodes_[nodeName]->setVisibilityMode (visibility);
        return true;
    }

    bool WindowsManager::setWireFrameMode (const char* nodeNameCorba,
            const char* wireFrameModeCorba)
    {
        const std::string nodeName (nodeNameCorba);
        const std::string wireFrameMode (wireFrameModeCorba);
        WireFrameMode wire = getWire (wireFrameMode);
        if (nodes_.find (nodeName) == nodes_.end ()) {
            std::cout << "Node \"" << nodeName << "\" doesn't exist."
                << std::endl;
            return false;
        }
        nodes_[nodeName]->setWireFrameMode (wire);
        return true;
    }

    bool WindowsManager::setLightingMode (const char* nodeNameCorba,
            const char* lightingModeCorba)
    {
        const std::string nodeName (nodeNameCorba);
        const std::string lightingMode (lightingModeCorba);
        LightingMode light = getLight (lightingMode);
        if (nodes_.find (nodeName) == nodes_.end ()) {
            std::cout << "Node \"" << nodeName << "\" doesn't exist."
                << std::endl;
            return false;
        }
        nodes_[nodeName]->setLightingMode (light);
        return true;
    }

    bool WindowsManager::startCapture (const WindowID windowId, const char* filename,
            const char* extension)
    {
        if (windowId >= 0 || windowId < windowManagers_.size ()) {
            windowManagers_[windowId]->startCapture
                (std::string (filename), std::string (extension));
            return true;
        }
        else {
            std::cout << "Window ID " << windowId
                << " doesn't exist." << std::endl;
            return false;
        }
    }

    bool WindowsManager::stopCapture (const WindowID windowId)
    {
        if (windowId >= 0 || windowId < windowManagers_.size ()) {
            windowManagers_[windowId]->stopCapture ();
            return true;
        }
        else {
            std::cout << "Window ID " << windowId
                << " doesn't exist." << std::endl;
            return false;
        }
    }

    bool WindowsManager::writeNodeFile (const WindowID windowId, const char* filename)
    {
        if (windowId >= 0 || windowId < windowManagers_.size ()) {
            return windowManagers_[windowId]->writeNodeFile (std::string (filename));
        }
        else {
            std::cout << "Window ID " << windowId
                << " doesn't exist." << std::endl;
            return false;
        }
    }

    WindowManagerPtr_t WindowsManager::getWindowManager (const WindowID wid)
    {
      if (wid >= 0 || wid < windowManagers_.size ()) {
        return windowManagers_[wid];
      }
      else {
        std::cout << "Window ID " << wid << " doesn't exist." << std::endl;
        return WindowManagerPtr_t ();
      }
    }
} // namespace graphics
