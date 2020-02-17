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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.6)

SET(PROJECT_NAME gepetto-viewer)
SET(PROJECT_DESCRIPTION "An user-friendly Graphical Interface")
SET(PROJECT_URL "https://github.com/Gepetto/gepetto-viewer")
SET(PROJECT_USE_CMAKE_EXPORT TRUE)

SET(CXX_DISABLE_WERROR TRUE)

INCLUDE(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/ide.cmake)
INCLUDE(cmake/apple.cmake)
INCLUDE(cmake/python.cmake)
INCLUDE(cmake/test.cmake)
INCLUDE(cmake-module/qt.cmake)
INCLUDE(cmake-module/osgqt.cmake)

COMPUTE_PROJECT_ARGS(PROJECT_ARGS LANGUAGES CXX)
PROJECT(${PROJECT_NAME} ${PROJECT_ARGS})

IF(APPLE)
  APPLY_DEFAULT_APPLE_CONFIGURATION()
ENDIF(APPLE)

# Declare Headers
SET(${PROJECT_NAME}_HEADERS_MOC
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/action-search-bar.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/bodytreewidget.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/dialog/configuration.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/dialog/dialogloadenvironment.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/dialog/dialogloadrobot.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/dialog/pluginmanagerdialog.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/ledindicator.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/mainwindow.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/node-action.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/osgwidget.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/selection-event.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/selection-handler.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/shortcut-factory.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/tree-item.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/windows-manager.hh
  )

SET(${PROJECT_NAME}_HEADERS_NO_MOC
  ${CMAKE_BINARY_DIR}/include/gepetto/gui/config-dep.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/color-map.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/fwd.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/pick-handler.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/qt-osg-keyboard.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/plugin-interface.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/safeapplication.hh
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/settings.hh

  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/config-osg.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/fwd.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/group-node.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-box.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-capsule.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-collada.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-cone.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-cylinder.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-face.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-ground.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-line.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-mesh.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-sphere.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-light.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-arrow.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/macros.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/node.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/node-drawable.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/urdf-parser.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/window-manager.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/windows-manager.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/leaf-node-xyzaxis.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/roadmap-viewer.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/node-rod.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/node-visitor.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/node-property.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/transform-writer.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/blender-geom-writer.h
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/OSGManipulator/keyboard-manipulator.h
)

SET(${PROJECT_NAME}_RESOURCES ${CMAKE_SOURCE_DIR}/res/images.qrc)

INSTALL(FILES fonts/arial.ttf
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/fonts
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE
)

FIND_PACKAGE(X11 REQUIRED)
INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
FIND_PACKAGE(OpenGL REQUIRED)

ADD_REQUIRED_DEPENDENCY("openscenegraph >= 3.2")
ADD_REQUIRED_DEPENDENCY("openthreads >= 2.6")
ADD_OSGQT_DEPENDENCY()
# Must be after ADD_OSGQT_DEPENDENCY
ADD_QT_DEPENDENCY()
ADD_OPTIONAL_DEPENDENCY("qgv")

# {{{ PythonQt settings
FINDPYTHON()

SET(CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/cmake-module/
  ${CMAKE_MODULE_PATH})
# If GEPETTO_GUI_HAS_PYTHONQT is set to TRUE, then PythonQt is required.
IF(DEFINED GEPETTO_GUI_HAS_PYTHONQT AND GEPETTO_GUI_HAS_PYTHONQT)
  FIND_PACKAGE(PythonQt REQUIRED)
ELSE()
  FIND_PACKAGE(PythonQt)
ENDIF()
# Declare option GEPETTO_GUI_HAS_PYTHONQT
# If GEPETTO_GUI_HAS_PYTHONQT was defined, this does not change its value.
IF (PythonQt_FOUND)
  SET(GEPETTO_GUI_HAS_PYTHONQT TRUE CACHE BOOL "Use PythonQt dependency")
  SET(BOOST_COMPONENTS python)
ELSE()
  SET(GEPETTO_GUI_HAS_PYTHONQT FALSE CACHE BOOL "Use PythonQt dependency")
  SET(BOOST_COMPONENTS python)
ENDIF()

IF(GEPETTO_GUI_HAS_PYTHONQT)
  SET (${PROJECT_NAME}_HEADERS_MOC
    ${${PROJECT_NAME}_HEADERS_MOC}
    ${CMAKE_SOURCE_DIR}/include/gepetto/gui/pythonwidget.hh)
ENDIF(GEPETTO_GUI_HAS_PYTHONQT)
# }}}

SET(BOOST_COMPONENTS ${BOOST_COMPONENTS} unit_test_framework system regex)
SEARCH_FOR_BOOST()
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})

FIND_PROGRAM(AVCONV avconv)
IF(NOT AVCONV)
  FIND_PROGRAM(FFMPEG ffmpeg)
  IF(FFMPEG)
    SET(AVCONV "${FFMPEG}")
  ELSE(FFMPEG)
    MESSAGE(WARNING "neither avconv nor ffmpeg have been found.")
    SET(AVCONV "avconv")
  ENDIF(FFMPEG)
ENDIF(AVCONV)

CONFIGURE_FILE(
  ${CMAKE_SOURCE_DIR}/include/gepetto/gui/config-dep.hh.in
  ${CMAKE_BINARY_DIR}/include/gepetto/gui/config-dep.hh
  @ONLY)

SET(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC}
  ${${PROJECT_NAME}_HEADERS_NO_MOC})

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(plugins)
ADD_SUBDIRECTORY(pyplugins)
ADD_SUBDIRECTORY(tests)

# {{{ Packaging
INSTALL(FILES cmake-module/gepetto-viewer-plugin.cmake
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake/
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE)
SET(PKG_CONFIG_EXTRA "${PKG_CONFIG_EXTRA}\ncmake_plugin=${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake/gepetto-viewer-plugin.cmake")

FOREACH(lib ${X11_INCLUDE_DIR})
  PKG_CONFIG_APPEND_CFLAGS("-I${lib}")
ENDFOREACH(lib ${X11_INCLUDE_DIR})
PKG_CONFIG_APPEND_LIBS_RAW(${X11_LIBRARIES})
PKG_CONFIG_APPEND_LIBS_RAW(${OPENGL_LIBRARIES})
PKG_CONFIG_APPEND_LIBS(${PROJECT_NAME})
# }}}

# vim: foldmethod=marker foldlevel=0
