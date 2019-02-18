#
# Copyright (c) 2014 LAAS-CNRS
# Authors: Mathieu Geisert
#
#
# This file is part of gepetto-viewer.
# SceneViewer is free software: you can redistribute it
# and/or modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation, either version
# 3 of the License, or (at your option) any later version.
#
# gepetto-viewer is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Lesser Public License for more details.  You should have
# received a copy of the GNU Lesser General Public License along with
# gepetto-viewer  If not, see
# <http://www.gnu.org/licenses/>.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CXX_DISABLE_WERROR TRUE)
INCLUDE(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/ide.cmake)
INCLUDE(cmake/apple.cmake)
INCLUDE(cmake/test.cmake)

SET(PROJECT_NAME gepetto-viewer)
SET(PROJECT_DESCRIPTION "An user-friendly Graphical Interface")
SET(PROJECT_URL "https://github.com/humanoid-path-planner/gepetto-viewer")

SETUP_PROJECT()

IF(APPLE)
  APPLY_DEFAULT_APPLE_CONFIGURATION()
ENDIF(APPLE)

# Declare Headers
SET(${PROJECT_NAME}_HEADERS
  include/gepetto/viewer/config-osg.h
  include/gepetto/viewer/group-node.h
  include/gepetto/viewer/leaf-node-box.h
  include/gepetto/viewer/leaf-node-capsule.h
  include/gepetto/viewer/leaf-node-collada.h
  include/gepetto/viewer/leaf-node-cone.h
  include/gepetto/viewer/leaf-node-cylinder.h
  include/gepetto/viewer/leaf-node-face.h
  include/gepetto/viewer/leaf-node-ground.h
  include/gepetto/viewer/leaf-node-line.h
  include/gepetto/viewer/leaf-node-mesh.h
  include/gepetto/viewer/leaf-node-sphere.h
  include/gepetto/viewer/leaf-node-light.h
  include/gepetto/viewer/leaf-node-arrow.h
  include/gepetto/viewer/macros.h
  include/gepetto/viewer/node.h
  include/gepetto/viewer/node-drawable.h
  include/gepetto/viewer/urdf-parser.h
  include/gepetto/viewer/window-manager.h
  include/gepetto/viewer/windows-manager.h
  include/gepetto/viewer/leaf-node-xyzaxis.h
  include/gepetto/viewer/roadmap-viewer.h
  include/gepetto/viewer/node-rod.h
  include/gepetto/viewer/node-visitor.h
  include/gepetto/viewer/node-property.h
  include/gepetto/viewer/transform-writer.h
  include/gepetto/viewer/blender-geom-writer.h
  include/gepetto/viewer/OSGManipulator/keyboard-manipulator.h
)
INSTALL(FILES fonts/arial.ttf
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/fonts
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE
)

FIND_PACKAGE(X11 REQUIRED)
INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
FIND_PACKAGE(OpenGL REQUIRED)

ADD_REQUIRED_DEPENDENCY("openscenegraph >= 3.2")
IF(${OPENSCENEGRAPH_VERSION} VERSION_GREATER "3.5.5")
  ADD_DEFINITIONS(-DOSG_3_5_6_OR_LATER)
  PKG_CONFIG_APPEND_CFLAGS("-DOSG_3_5_6_OR_LATER")
  ADD_REQUIRED_DEPENDENCY("openscenegraph-osgQt5")
  SET(USE_QT4 FALSE CACHE BOOL "Use Qt4 instead of Qt5")
ELSE()
  ADD_REQUIRED_DEPENDENCY("openscenegraph-osgQt")

  IF(NOT DEFINED USE_QT4)
    FILE(READ "${OPENSCENEGRAPH_OSGQT_INCLUDEDIR}/osgQt/Version"
      osgqt_version_content
      LIMIT 500)
    STRING(FIND ${osgqt_version_content} "#define OSGQT_QT_VERSION 4" osgqt_qt_version4_idx)
    STRING(FIND ${osgqt_version_content} "#define OSGQT_QT_VERSION 5" osgqt_qt_version5_idx)
    IF(osgqt_qt_version4_idx EQUAL -1 AND osgqt_qt_version5_idx EQUAL -1)
    ELSEIF(osgqt_qt_version4_idx EQUAL -1 )
      # QT5 only
      SET(USE_QT4 FALSE CACHE BOOL "Use Qt4 instead of Qt5")
    ELSEIF(osgqt_qt_version5_idx EQUAL -1 )
      # QT4 only
      SET(USE_QT4 TRUE CACHE BOOL "Use Qt4 instead of Qt5")
    ELSE()
      # Both found.
      MESSAGE(FATAL_ERROR "Could not compute Qt version. Please set USE_QT4 variable manually")
    ENDIF()
  ENDIF(NOT DEFINED USE_QT4)
ENDIF()

IF(USE_QT4)
  FIND_PACKAGE(Qt4 REQUIRED QtXml)
  SET(PROJECT_QT_LIBS Qt4::QtXml)
  SET(PKG_CONFIG_EXTRA "qtversion=${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
ELSE()
  FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Xml)
  SET(PROJECT_QT_LIBS Qt5::Xml)
  SET(PKG_CONFIG_EXTRA "qtversion=${Qt5Core_VERSION}")
ENDIF()


SET(BOOST_COMPONENTS "unit_test_framework")
SEARCH_FOR_BOOST()
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)

FOREACH(lib ${X11_INCLUDE_DIR})
  PKG_CONFIG_APPEND_CFLAGS("-I${lib}")
ENDFOREACH(lib ${X11_INCLUDE_DIR})
PKG_CONFIG_APPEND_LIBS_RAW(${X11_LIBRARIES})
PKG_CONFIG_APPEND_LIBS_RAW(${OPENGL_LIBRARIES})
PKG_CONFIG_APPEND_LIBS(${PROJECT_NAME})

SETUP_PROJECT_FINALIZE()
