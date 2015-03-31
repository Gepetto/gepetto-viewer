// Copyright (c) 2014, LAAS-CNRS
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

#ifndef SCENEVIEWER_WINDOWMANAGERS_HH
#define SCENEVIEWER_WINDOWMANAGERS_HH

#include <gepetto/viewer/window-manager.h>
#include <boost/thread/mutex.hpp>

namespace graphics {

    struct NodeConfiguration {
        NodePtr_t node;
        osgVector3 position;
        osgQuat quat;
    };

    DEF_CLASS_SMART_PTR(WindowsManager)

    class WindowsManager
    {
        public:
            // Typedef for position and color values.
            typedef osg::Vec3f::value_type value_type;
            typedef unsigned int WindowID;

        private:
            typedef std::vector <WindowManagerPtr_t> WindowManagerVector_t;
            WindowManagerVector_t windowManagers_;
            std::map<std::string, NodePtr_t> nodes_;
            std::map<std::string, GroupNodePtr_t> groupNodes_;
            boost::mutex mtx_;
            int rate_;
            std::list<NodeConfiguration> newNodeConfigurations_;

            static osgVector4 getColor(const std::string& colorName);
            static osgVector4 getColor(const float* color);
            static std::string parentName(const std::string& name);
            static VisibilityMode getVisibility(const std::string& visibilityName);
            static WireFrameMode getWire(const std::string& wireName);
            static LightingMode getLight(const std::string& lightName);
            void initParent(const std::string& nodeName, NodePtr_t node);
            void addNode(const std::string& nodeName, NodePtr_t node);
            void addGroup(const std::string& groupName, GroupNodePtr_t group);
            void threadRefreshing(WindowManagerPtr_t window);
            static osgQuat corbaConfToOsgQuat(const value_type* configurationCorba);
            static osgVector3 corbaConfToOsgVec3(const value_type* configurationCorba);

        protected:
            /**
              \brief Default constructor
              */
            WindowsManager ();

        public:
            static WindowsManagerPtr_t create ();

            virtual void getNodeList();
            virtual void getWindowList();

            virtual bool setRate(const int& rate);
            virtual void refresh();

            virtual WindowID createWindow(const char* windowNameCorba, osg::GraphicsContext* gc = 0);

            virtual void createScene(const char* sceneNameCorba);
            virtual void createSceneWithFloor(const char* sceneNameCorba);
            virtual bool addSceneToWindow(const char* sceneNameCorba, const WindowID windowId);


            virtual bool addBox(const char* boxName, float boxSize1, float boxSize2, float boxSize3, const value_type* color);

            virtual bool addCapsule(const char* capsuleName, float radius, float height, const value_type* color);

            virtual bool addMesh(const char* meshNameCorba, const char* meshPathCorba);

            virtual bool addCone(const char* coneName, float radius, float height, const value_type* color);

            virtual bool addCylinder(const char* cylinderName, float radius, float height, const value_type* color);

            virtual bool addSphere(const char* sphereName, float radius, const value_type* color);

            virtual bool addLine(const char* lineName, const value_type* pos1, const value_type* pos2, const value_type* color);

            virtual bool addSquareFace(const char* faceName, const value_type* pos1, const value_type* pos2, const value_type* pos3, const value_type* pos4, const value_type* color);
            virtual bool addTriangleFace(const char* faceName, const value_type* pos1, const value_type* pos2, const value_type* pos3, const value_type* color);

            virtual bool addURDF(const char* urdfNameCorba, const char* urdfPathCorba, const char* urdfPackagePathCorba);

            virtual bool addUrdfCollision (const char* urdfNameCorba,
                    const char* urdfPathCorba,
                    const char* urdfPackagePathCorba) ;

            virtual void addUrdfObjects (const char* urdfNameCorba,
                    const char* urdfPathCorba,
                    const char* urdfPackagePathCorba,
                    bool visual) ;

            virtual bool createGroup(const char* groupNameCorba);
            virtual bool addToGroup(const char* nodeNameCorba, const char* groupNameCorba);

            virtual bool applyConfiguration(const char* nodeNameCorba, const value_type* configuration);

            virtual bool addLandmark(const char* nodeNameCorba, float size);
            virtual bool deleteLandmark(const char* nodeNameCorba);

            virtual bool setVisibility(const char* nodeNameCorba, const char* visibilityModeCorba);
            virtual bool setWireFrameMode(const char* nodeNameCorba, const char* wireFrameModeCorba);
            virtual bool setLightingMode(const char* nodeNameCorba, const char* lightingModeCorba);

            virtual bool startCapture (const WindowID windowId, const char* filename,
                    const char* extension);
            virtual bool stopCapture (const WindowID windowId);
            virtual bool writeNodeFile (const WindowID windowId, const char* filename);

            WindowManagerPtr_t getWindowManager (const WindowID wid);
            GroupNodePtr_t getScene (const std::string sceneName);
            NodePtr_t getNode (const std::string nodeName);
    };
} /* namespace graphics */

#endif /* SCENEVIEWER_WINDOWMANAGERS_HH */
