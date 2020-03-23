// Copyright (c) 2019, LAAS-CNRS
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

#include "../../src/gui/python-bindings.hh"

#ifndef Q_MOC_RUN
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#endif

#include <gepetto/viewer/windows-manager.h>
#include <gepetto/gui/windows-manager.hh>

namespace bp = boost::python;
namespace gv = gepetto::viewer;
namespace gg = gepetto::gui;
using boost::noncopyable;

#define GV_DEF(func) .def (#func, &WindowsManager::func)
#define GV_DEF0(func,ret)                                                      \
  .def (#func,                                                                 \
      static_cast<ret (WindowsManager::*)()>                                   \
      (&WindowsManager::func))
#define GV_DEF1(func,ret,arg0)                                                 \
  .def (#func,                                                                 \
      static_cast<ret (WindowsManager::*)(arg0)>                               \
      (&WindowsManager::func))
#define GV_DEF2(func,ret,arg0,arg1)                                            \
  .def (#func,                                                                 \
      static_cast<ret (WindowsManager::*)(arg0,arg1)>                          \
      (&WindowsManager::func))
#define GV_DEF3(func,ret,arg0,arg1,arg2)                                       \
  .def (#func,                                                                 \
      static_cast<ret (WindowsManager::*)(arg0,arg1,arg2)>                     \
      (&WindowsManager::func))
#define GV_DEF4(func,ret,arg0,arg1,arg2,arg3)                                  \
  .def (#func,                                                                 \
      static_cast<ret (WindowsManager::*)(arg0,arg1,arg2,arg3)>                \
      (&WindowsManager::func))

struct to_python_converters {
  static PyObject* convert(const osgVector3& v)
  {
    return bp::incref(bp::make_tuple (v[0],v[1],v[2]).ptr());
  }

  static PyObject* convert(const osgQuat& v)
  {
    return bp::incref(bp::make_tuple (v[0],v[1],v[2],v[3]).ptr());
  }

  static PyObject* convert(const gv::Configuration& v)
  {
    return bp::incref(bp::make_tuple (
        v.position[0],v.position[1],v.position[2],
        v.quat[0],v.quat[1],v.quat[2],v.quat[3]).ptr());
  }
};

template <typename V>
inline void set(V& v, int i, const double& d) { v[i] = (float)d; }
inline void set(gv::Configuration& v, int i, const double& d) {
  if (i < 3) v.position[i  ] = (float)d;
  else       v.quat    [i-3] = d;
}

template <typename V, int N>
struct from_python_converters
{
    from_python_converters()
    {
      boost::python::converter::registry::push_back(
        &convertible,
        &construct,
        boost::python::type_id<V>());
    }

    // Determine if obj_ptr can be converted in a QString
    static void* convertible(PyObject* obj_ptr)
    {
      if (!PyIter_Check(obj_ptr)) return 0;
      PyObject *iterator = PyObject_GetIter(obj_ptr);
      PyObject* item;
      for (int i = 0; i < N; ++i)
      {
        item = PyIter_Next(iterator);
        if (item==NULL) { Py_DECREF(iterator); return 0; }
        bool isFloat = PyFloat_Check(item);
        Py_DECREF(item);
        if (!isFloat  ) { Py_DECREF(iterator); return 0; }
      }
      item = PyIter_Next(iterator);
      Py_DECREF(iterator);
      if (item != NULL) {
        Py_DECREF(item);
        return 0;
      }
      return obj_ptr;
    }

    // Convert obj_ptr into a QString
    static void construct(
    PyObject* obj_ptr,
    boost::python::converter::rvalue_from_python_stage1_data* data)
    {
      // Grab pointer to memory into which to construct the new QString
      void* storage = (
        (boost::python::converter::rvalue_from_python_storage<V>*)
        data)->storage.bytes;

      // in-place construct the new QString using the character data
      // extraced from the python object
      new (storage) V;

      V& value (* (V*)storage );

      PyObject *iterator = PyObject_GetIter(obj_ptr);
      for (int i = 0; i < N; ++i)
      {
        PyObject* item = PyIter_Next(iterator);
        set(value, i, PyFloat_AsDouble(item));
        Py_DECREF(item);
      }
      Py_DECREF(iterator);

      // Stash the memory chunk pointer for later use by boost.python
      data->convertible = storage;
    }
};

void exposeOSG ()
{
  bp::class_<std::vector<std::string> >("string_vector")
    .def(bp::vector_indexing_suite<std::vector<std::string> >())
    ;
  bp::to_python_converter<osgVector3            , to_python_converters>();
  bp::to_python_converter<osgQuat               , to_python_converters>();
  bp::to_python_converter<gv::Configuration     , to_python_converters>();

  from_python_converters<osgVector3       , 3>();
  from_python_converters<osgQuat          , 4>();
  from_python_converters<gv::Configuration, 7>();
}

void exposeGV ()
{
  typedef gepetto::viewer::WindowsManager WindowsManager;

  bp::class_ <WindowsManager, noncopyable>
    ("WindowsManagerBase", bp::no_init)
    GV_DEF (getNodeList)
    GV_DEF (getGroupNodeList)
    GV_DEF (getSceneList)
    GV_DEF (getWindowList)

    GV_DEF (getWindowID)

    GV_DEF (createScene)
    GV_DEF (createSceneWithFloor)
    GV_DEF (addSceneToWindow)

    GV_DEF (attachCameraToNode)
    GV_DEF (detachCamera)

    GV_DEF (nodeExists)

    GV_DEF (addFloor)
    GV_DEF (addBox)
    GV_DEF (addCapsule)
    GV_DEF (resizeCapsule)
    GV_DEF (addArrow)
    GV_DEF (resizeArrow)
    GV_DEF (addRod)
    GV_DEF (addMesh)
    GV_DEF (removeLightSources)
    GV_DEF (addCone)
    GV_DEF (addCylinder)
    GV_DEF (addSphere)
    GV_DEF (addLight)
    GV_DEF (addLine)
    GV_DEF (setLineStartPoint)
    GV_DEF (setLineEndPoint)
    GV_DEF (setLineExtremalPoints)
    GV_DEF (addCurve)
    GV_DEF (setCurvePoints)
    GV_DEF (setCurveMode)
    GV_DEF (setCurvePointsSubset)
    GV_DEF (setCurveLineWidth)
    GV_DEF (addSquareFace)
    GV_DEF (setTexture)
    GV_DEF (addTriangleFace)
    GV_DEF (addXYZaxis)

    GV_DEF (createRoadmap)
    GV_DEF (addEdgeToRoadmap)
    GV_DEF (addNodeToRoadmap)

    GV_DEF2 (addURDF, bool, const std::string&, const std::string&)
    GV_DEF3 (addURDF, bool, const std::string&, const std::string&, const std::string&)
    GV_DEF2 (addUrdfCollision, bool, const std::string&, const std::string&)
    GV_DEF3 (addUrdfCollision, bool, const std::string&, const std::string&, const std::string&)
    GV_DEF3 (addUrdfObjects, void, const std::string&, const std::string&, bool)
    GV_DEF4 (addUrdfObjects, void, const std::string&, const std::string&, const std::string&, bool)

    GV_DEF (createGroup)
    GV_DEF (addToGroup)
    GV_DEF (removeFromGroup)
    GV_DEF (deleteNode)

    GV_DEF (applyConfiguration)
    GV_DEF (applyConfigurations)

    GV_DEF (addLandmark)
    GV_DEF (deleteLandmark)

    //virtual Configuration getStaticTransform (const std::string& nodeName) const;
    GV_DEF (setStaticTransform)

    GV_DEF (setVisibility)
    GV_DEF2 (setScale, bool, const std::string&, const osgVector3&)
    GV_DEF2 (setScale, bool, const std::string&, const float&)
    GV_DEF2 (setScale, bool, const std::string&, const int&)
    GV_DEF (setColor)
    GV_DEF (setWireFrameMode)
    GV_DEF (setLightingMode)
    GV_DEF (setHighlight)
    GV_DEF2 (setAlpha, bool, const std::string&, const float&)
    GV_DEF2 (setAlpha, bool, const std::string&, const int&)

    GV_DEF (setCaptureTransform)
    GV_DEF (captureTransformOnRefresh)
    GV_DEF (captureTransform)
    GV_DEF (writeBlenderScript)
    GV_DEF (writeNodeFile)
    GV_DEF (writeWindowFile)
    GV_DEF (setBackgroundColor1)
    GV_DEF (setBackgroundColor2)
    GV_DEF (getCameraTransform)
    GV_DEF (setCameraTransform)

    GV_DEF (getPropertyNames)
    GV_DEF (getPropertyTypes)

    GV_DEF (getStringProperty)
    GV_DEF (setStringProperty)
    GV_DEF (getVector2Property)
    GV_DEF (setVector2Property)
    GV_DEF (getVector3Property)
    GV_DEF (setVector3Property)
    GV_DEF (getColorProperty)
    GV_DEF (setColorProperty)
    GV_DEF (getFloatProperty)
    GV_DEF (setFloatProperty)
    GV_DEF (getBoolProperty)
    GV_DEF (setBoolProperty)
    GV_DEF (getIntProperty)
    GV_DEF (setIntProperty)

    //WindowManagerPtr_t getWindowManager (const WindowID wid, bool throwIfDoesntExist = false) const;
    //GroupNodePtr_t getGroup (const std::string groupName, bool throwIfDoesntExist = false) const;
    //NodePtr_t getNode (const std::string& nodeName, bool throwIfDoesntExist = false) const;
    //Configuration getNodeGlobalTransform(const std::string nodeName) const;
    ;
}

void exposeGG ()
{
  typedef gepetto::gui::WindowsManager WindowsManager;
  using boost::noncopyable;

  bp::class_ <WindowsManager, bp::bases<gv::WindowsManager>, noncopyable>
    ("WindowsManager", bp::no_init)
    GV_DEF (captureFrame)
    GV_DEF (startCapture)
    GV_DEF (stopCapture)

    GV_DEF (refresh)

    GV_DEF1 (createWindow, WindowsManager::WindowID, const std::string&)
    ;
}

#undef GV_DEF
#undef GV_DEF0
#undef GV_DEF1
#undef GV_DEF2
#undef GV_DEF3
#undef GV_DEF4
