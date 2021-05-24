#
# Copyright (c) 2014, 2020 LAAS-CNRS
# Authors: Mathieu Geisert, Joseph Mirabel, Guilhem Saurel
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

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

SET(PROJECT_NAME gepetto-viewer)
SET(PROJECT_DESCRIPTION "An user-friendly Graphical Interface")
SET(PROJECT_URL "https://github.com/Gepetto/gepetto-viewer")

SET(PROJECT_USE_CMAKE_EXPORT TRUE)
SET(PROJECT_USE_KEYWORD_LINK_LIBRARIES TRUE)
SET(CXX_DISABLE_WERROR TRUE)
SET(DOXYGEN_USE_TEMPLATE_CSS TRUE)

INCLUDE(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/ide.cmake)
INCLUDE(cmake/apple.cmake)
INCLUDE(cmake/python.cmake)
INCLUDE(cmake/cxx-standard.cmake)
INCLUDE(cmake-module/qt.cmake)
INCLUDE(cmake-module/osgqt.cmake)

COMPUTE_PROJECT_ARGS(PROJECT_ARGS LANGUAGES CXX)
PROJECT(${PROJECT_NAME} ${PROJECT_ARGS})

CHECK_MINIMAL_CXX_STANDARD(11 ENFORCE)

OPTION(ENABLE_QT_DEBUG "Enable Qt debug output." FALSE)

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
  ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/properties.h
)

SET(${PROJECT_NAME}_RESOURCES ${CMAKE_SOURCE_DIR}/res/images.qrc)

INSTALL(FILES fonts/arial.ttf
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/fonts
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE
)

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
  FIND_PACKAGE(PythonQt REQUIRED COMPONENTS QtAll REQUIRED)
ELSE()
  FIND_PACKAGE(PythonQt COMPONENTS QtAll)
ENDIF()
# Declare option GEPETTO_GUI_HAS_PYTHONQT
# If GEPETTO_GUI_HAS_PYTHONQT was defined, this does not change its value.
IF (PythonQt_FOUND AND PythonQt_QtAll_FOUND)
  SET(GEPETTO_GUI_HAS_PYTHONQT TRUE CACHE BOOL "Use PythonQt dependency")
ELSE()
  SET(GEPETTO_GUI_HAS_PYTHONQT FALSE CACHE BOOL "Use PythonQt dependency")
ENDIF()

IF(GEPETTO_GUI_HAS_PYTHONQT)
  SET (${PROJECT_NAME}_HEADERS_MOC
    ${${PROJECT_NAME}_HEADERS_MOC}
    ${CMAKE_SOURCE_DIR}/include/gepetto/gui/pythonwidget.hh)
ENDIF(GEPETTO_GUI_HAS_PYTHONQT)
# }}}

FIND_PACKAGE(Boost REQUIRED COMPONENTS COMPONENTS unit_test_framework)
SEARCH_FOR_BOOST_PYTHON(REQUIRED)

# Check if we are on a platform with ffmpeg or avconv
FIND_PROGRAM(FFMPEG ffmpeg)
IF(NOT FFMPEG)
  FIND_PROGRAM(AVCONV avconv)
  IF(AVCONV)
    SET(FFMPEG "${AVCONV}")
  ELSE(AVCONV)
    MESSAGE(WARNING "neither ffmpeg nor avconv have been found.")
    # Fallback to old behavior:
    # ffmpeg / avconv non-availability at configure time
    # doesn't mean anything at run time.
    SET(FFMPEG "ffmpeg")
  ENDIF(FFMPEG)
ENDIF(NOT FFMPEG)

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
SET(GEPETTO_VIEWER_CMAKE_PLUGIN "${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME}/${PROJECT_NAME}-plugin.cmake")
SET(PKG_CONFIG_EXTRA "${PKG_CONFIG_EXTRA}\ncmake_plugin=${GEPETTO_VIEWER_CMAKE_PLUGIN}")
SET(PACKAGE_EXTRA_MACROS "${PACKAGE_EXTRA_MACROS}\nset(GEPETTO_VIEWER_CMAKE_PLUGIN ${GEPETTO_VIEWER_CMAKE_PLUGIN})")
SET(PACKAGE_EXTRA_MACROS "${PACKAGE_EXTRA_MACROS}\nINCLUDE(${GEPETTO_VIEWER_CMAKE_PLUGIN})")
SET(PACKAGE_EXTRA_MACROS "${PACKAGE_EXTRA_MACROS}\nset(GEPETTO_VIEWER_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)")
INSTALL(FILES cmake-module/${PROJECT_NAME}-plugin.cmake
  DESTINATION lib/cmake/${PROJECT_NAME}
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE)

PKG_CONFIG_APPEND_LIBS(${PROJECT_NAME})
# }}}

INSTALL(FILES package.xml DESTINATION share/${PROJECT_NAME})
# vim: foldmethod=marker foldlevel=0
