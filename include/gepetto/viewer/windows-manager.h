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

#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>

#include <gepetto/viewer/config-osg.h>
#include <gepetto/viewer/transform-writer.h>

namespace graphics {

    typedef OpenThreads::Mutex Mutex;
    typedef OpenThreads::ScopedLock<Mutex> ScopedLock;

    DEF_CLASS_SMART_PTR(Node)
    DEF_CLASS_SMART_PTR(GroupNode)
    DEF_CLASS_SMART_PTR(WindowManager)
    DEF_CLASS_SMART_PTR(RoadmapViewer)

    DEF_CLASS_SMART_PTR(WindowsManager)

    struct Configuration {
        osgVector3 position;
        osgQuat quat;
        Configuration() {}
        /// \param XYZW when false, the 4 last parameters are a quaternion (w,x,y,z)
        ///             otherwise, a quaternion (x,y,z,w)
        explicit Configuration(const float* a, bool XYZW)
          : position(a[0],a[1],a[2])
          , quat(a[(XYZW ? 3 : 4)],
                 a[(XYZW ? 4 : 5)],
                 a[(XYZW ? 5 : 6)],
                 a[(XYZW ? 6 : 3)]) {}
        Configuration(const osgVector3& p, const osgQuat& q) : position(p), quat(q) {}
    };
    struct NodeConfiguration : Configuration {
        NodePtr_t node;
    };

    struct BlenderFrameCapture {
      typedef std::vector<NodePtr_t> Nodes_t;
      osg::ref_ptr < TransformWriterVisitor > writer_visitor_;
      Nodes_t nodes_;
      BlenderFrameCapture ();
      void captureFrame ();
    };

    class WindowsManager
    {
        public:
            // Typedef for position and color values.
            typedef osg::Vec3f::value_type value_type;
            typedef osgVector4 Color_t;
            typedef ::osg::Vec3ArrayRefPtr Vec3ArrayPtr_t;
            typedef unsigned int WindowID;

            typedef std::map <std::string, WindowID> WindowIDMap_t;
            WindowIDMap_t windowIDmap_;

        private:
            typedef std::vector <WindowManagerPtr_t> WindowManagerVector_t;
            WindowManagerVector_t windowManagers_;
            std::map<std::string, NodePtr_t> nodes_;
            std::map<std::string, GroupNodePtr_t> groupNodes_;
            std::map<std::string, RoadmapViewerPtr_t> roadmapNodes_;
            Mutex osgFrameMtx_;
            BlenderFrameCapture blenderCapture_;

            static osgVector4 getColor(const std::string& colorName);
            static std::string parentName(const std::string& name);
            static VisibilityMode getVisibility(const std::string& visibilityName);
            static WireFrameMode getWire(const std::string& wireName);
            static LightingMode getLight(const std::string& lightName);
            NodePtr_t find (const std::string name, GroupNodePtr_t group = GroupNodePtr_t());
            void initParent(NodePtr_t node, GroupNodePtr_t parent);
            bool loadUDRF(const std::string& urdfName, const std::string& urdfPath,
                bool visual, bool linkFrame);

        protected:
            /**
              \brief Default constructor
              */
            WindowsManager ();
            WindowID addWindow (std::string winName, WindowManagerPtr_t newWindow);

            typedef std::vector<NodeConfiguration> NodeConfigurations_t;
            Mutex configListMtx_;
            NodeConfigurations_t newNodeConfigurations_;
            bool autoCaptureTransform_;
            void refreshConfigs (const NodeConfigurations_t& configs);

            template <typename Iterator, typename NodeContainer_t> 
              std::size_t getNodes
              (const Iterator& begin, const Iterator& end, NodeContainer_t& nodes);

            /// Warning, the mutex should be locked before and unlocked after this opertations.
            void addNode (const std::string& nodeName, NodePtr_t node, bool guessParent = false);
            virtual void addNode (const std::string& nodeName, NodePtr_t node, GroupNodePtr_t parent);
            void addGroup(const std::string& groupName, GroupNodePtr_t group, bool guessParent = false);
            virtual void addGroup(const std::string& groupName, GroupNodePtr_t group, GroupNodePtr_t parent);

        public:
            static WindowsManagerPtr_t create ();

            virtual ~WindowsManager () {};

            virtual std::vector<std::string> getNodeList();
            virtual std::vector<std::string> getGroupNodeList(const std::string& group);
            virtual std::vector<std::string> getSceneList();
            virtual std::vector<std::string> getWindowList();

            /// Return the mutex to be locked before modifying the scene.
            Mutex& osgFrameMutex () {
              return osgFrameMtx_;
            }

            virtual WindowID getWindowID (const std::string& windowName);

            virtual void createScene(const std::string& sceneName);
            virtual void createSceneWithFloor(const std::string& sceneName);
            virtual bool addSceneToWindow(const std::string& sceneName, const WindowID windowId);

            virtual bool attachCameraToNode(const std::string& nodeName, const WindowID windowId);
            virtual bool detachCamera(const WindowID windowId);

            virtual bool nodeExists (const std::string& name);

            virtual bool addFloor(const std::string& floorName);

            /// Add a box in the scene
            /// \param boxName name of the box,
            /// \param boxSize1, boxSize2, boxSize3 lengths of the box along
            ///        axes x, y, z,
            /// \param color the color of the box.
            virtual bool addBox(const std::string& boxName, const float& boxSize1, const float& boxSize2, const float& boxSize3, const Color_t& color);

            virtual bool addCapsule(const std::string& capsuleName, float radius, float height, const Color_t& color);

            virtual  bool addArrow (const std::string& arrowName,const float radius, const float length,  const Color_t& color);

            virtual bool addRod (const std::string& rodName, const Color_t& color,const float radius,const float length, short maxCapsule);

            virtual bool resizeCapsule(const std::string& capsuleName, float newHeight) throw (std::exception);
            virtual bool resizeArrow(const std::string& arrowName ,float newRadius, float newLength) throw(std::exception);

            virtual bool addMesh(const std::string& meshName, const std::string& meshPath);

            virtual bool addCone(const std::string& coneName, float radius, float height, const Color_t& color);

            virtual bool addCylinder(const std::string& cylinderName, float radius, float height, const Color_t& color);

            virtual bool addSphere(const std::string& sphereName, float radius, const Color_t& color);

            virtual bool addLight(const std::string& lightName, const WindowID wid, float radius, const Color_t& color);

            virtual bool addLine(const std::string& lineName, const osgVector3& pos1, const osgVector3& pos2, const Color_t& color);
            virtual bool setLineStartPoint(const std::string& lineName, const osgVector3& pos1);
            virtual bool setLineEndPoint(const std::string& lineName, const osgVector3& pos2);
            virtual bool setLineExtremalPoints(const std::string& lineName, const osgVector3& pos1, const osgVector3& pos2);

            virtual bool addCurve(const std::string& curveName, const Vec3ArrayPtr_t& pos, const Color_t& color);
      
            virtual bool setCurvePoints(const std::string& curveName, const Vec3ArrayPtr_t& pos);

            /// \param mode See LeafNodeLine::setMode for possible values
            virtual bool setCurveMode (const std::string& curveName, const GLenum mode);
            virtual bool setCurvePointsSubset (const std::string& curveName, const int first, const std::size_t count);
            virtual bool setCurveLineWidth (const std::string& curveName, const float& width);

            virtual bool addSquareFace(const std::string& faceName, const osgVector3& pos1, const osgVector3& pos2, const osgVector3& pos3, const osgVector3& pos4, const Color_t& color);
            virtual bool setTexture (const std::string& nodeName, const std::string& filename);
            virtual bool addTriangleFace(const std::string& faceName, const osgVector3& pos1, const osgVector3& pos2, const osgVector3& pos3, const Color_t& color);
            virtual bool addXYZaxis (const std::string& nodeName, const Color_t& color, float radius, float sizeAxis);

            virtual bool createRoadmap(const std::string& name,const Color_t& colorNode, float radius, float sizeAxis, const Color_t& colorEdge);

            virtual bool addEdgeToRoadmap(const std::string& nameRoadmap, const osgVector3& posFrom, const osgVector3& posTo);

            virtual bool addNodeToRoadmap(const std::string& nameRoadmap, const Configuration& configuration);

            virtual bool addURDF(const std::string& urdfName, const std::string& urdfPath);
            /// \deprecated Argument urdfPackagePathCorba is ignored.
            virtual bool addURDF(const std::string& urdfName, const std::string& urdfPath, const std::string& urdfPackagePath);

            virtual bool addUrdfCollision (const std::string& urdfName,
                    const std::string& urdfPath) ;
            /// \deprecated Argument urdfPackagePathCorba is ignored.
            virtual bool addUrdfCollision (const std::string& urdfName,
                    const std::string& urdfPath,
                    const std::string& urdfPackagePath) ;

            virtual void addUrdfObjects (const std::string& urdfName,
                    const std::string& urdfPath,
                    bool visual) ;
            /// \deprecated Argument urdfPackagePathCorba is ignored.
            virtual void addUrdfObjects (const std::string& urdfName,
                    const std::string& urdfPath,
                    const std::string& urdfPackagePath,
                    bool visual) ;

            virtual bool createGroup(const std::string& groupName);
            virtual bool addToGroup(const std::string& nodeName, const std::string& groupName);
            virtual bool removeFromGroup (const std::string& nodeName, const std::string& groupName);
            virtual bool deleteNode (const std::string& nodeName, bool all);

            virtual bool applyConfiguration(const std::string& nodeName, const Configuration& configuration);
            virtual bool applyConfigurations(const std::vector<std::string>& nodeName, const std::vector<Configuration>& configuration);

            virtual bool addLandmark(const std::string& nodeName, float size);
            virtual bool deleteLandmark(const std::string& nodeName);

            virtual Configuration getStaticTransform (const std::string& nodeName) const;
            virtual bool setStaticTransform (const std::string& nodeName, const Configuration& transform);

            virtual bool setVisibility(const std::string& nodeName, const std::string& visibilityMode);
            virtual bool setScale(const std::string& nodeName, const osgVector3& scale);
            virtual bool setScale(const std::string& nodeName, const float& scale);
            virtual bool setScale(const std::string& nodeName, const int& scalePercentage);
            virtual bool setColor(const std::string& nodeName, const Color_t& color);
            virtual bool setWireFrameMode(const std::string& nodeName, const std::string& wireFrameMode);
            virtual bool setLightingMode(const std::string& nodeName, const std::string& lightingMode);
            virtual bool setHighlight(const std::string& nodeName, int state);
            virtual bool setAlpha(const std::string& nodeName, const float& alpha);
            virtual bool setAlpha(const std::string& nodeName, const int& alphaPercentage);

            virtual bool setCaptureTransform (const std::string& filename, const std::vector<std::string>& nodename);
            virtual void captureTransformOnRefresh (bool autoCapture);
            virtual void captureTransform ();
            virtual bool writeBlenderScript (const std::string& filename, const std::vector<std::string>& nodename);
            virtual bool writeNodeFile (const std::string& nodename, const std::string& filename);
            virtual bool writeWindowFile (const WindowID windowId, const std::string& filename);
            virtual bool setBackgroundColor1(const WindowID windowId,const Color_t& color);
            virtual bool setBackgroundColor2(const WindowID windowId,const Color_t& color);
            virtual Configuration getCameraTransform(const WindowID windowId);
            virtual bool setCameraTransform(const WindowID windowId, const Configuration &configuration);

            virtual std::vector<std::string> getPropertyNames(const std::string& nodeName) const;
            virtual std::vector<std::string> getPropertyTypes(const std::string& nodeName) const;

            template <typename Property_t> Property_t getProperty (const std::string& nodeName, const std::string& propName) const;
            template <typename Property_t> void setProperty (const std::string& nodeName, const std::string& propName, const Property_t& value);

            virtual std::string getStringProperty(const std::string& nodeName, const std::string& propName) const;
            virtual void setStringProperty(const std::string& nodeName, const std::string& propName, const std::string& value);
            virtual osgVector3 getVector3Property(const std::string& nodeName, const std::string& propName) const;
            virtual void setVector3Property(const std::string& nodeName, const std::string& propName, const osgVector3& value);
            virtual osgVector4 getColorProperty(const std::string& nodeName, const std::string& propName) const;
            virtual void setColorProperty(const std::string& nodeName, const std::string& propName, const osgVector4& value);
            virtual float getFloatProperty(const std::string& nodeName, const std::string& propName) const;
            virtual void setFloatProperty(const std::string& nodeName, const std::string& propName, const float& value);
            virtual bool getBoolProperty(const std::string& nodeName, const std::string& propName) const;
            virtual void setBoolProperty(const std::string& nodeName, const std::string& propName, const bool& value);
            virtual int getIntProperty(const std::string& nodeName, const std::string& propName) const;
            virtual void setIntProperty(const std::string& nodeName, const std::string& propName, const int& value);

            WindowManagerPtr_t getWindowManager (const WindowID wid, bool throwIfDoesntExist = false) const;
            GroupNodePtr_t getGroup (const std::string groupName, bool throwIfDoesntExist = false) const;
            NodePtr_t getNode (const std::string& nodeName, bool throwIfDoesntExist = false) const;
            Configuration getNodeGlobalTransform(const std::string nodeName) const;
    };
} /* namespace graphics */

#endif /* SCENEVIEWER_WINDOWMANAGERS_HH */
