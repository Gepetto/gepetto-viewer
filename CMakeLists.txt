#
# Copyright (c) 2014, 2020 LAAS-CNRS Authors: Mathieu Geisert, Joseph Mirabel,
# Guilhem Saurel
#
# This file is part of gepetto-viewer. SceneViewer is free software: you can
# redistribute it and/or modify it under the terms of the GNU Lesser General
# Public License as published by the Free Software Foundation, either version 3
# of the License, or (at your option) any later version.
#
# gepetto-viewer is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Lesser Public License for more
# details.  You should have received a copy of the GNU Lesser General Public
# License along with gepetto-viewer  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME gepetto-viewer)
set(PROJECT_DESCRIPTION "An user-friendly Graphical Interface")
set(PROJECT_URL "https://github.com/Gepetto/gepetto-viewer")

set(PROJECT_USE_CMAKE_EXPORT TRUE)
set(PROJECT_USE_KEYWORD_LINK_LIBRARIES TRUE)
set(CXX_DISABLE_WERROR TRUE)
set(DOXYGEN_USE_TEMPLATE_CSS TRUE)
set(PYTHON_EXPORT_DEPENDENCY TRUE)

# Check if the submodule cmake have been initialized
set(JRL_CMAKE_MODULES "${CMAKE_CURRENT_LIST_DIR}/cmake")
if(EXISTS "${JRL_CMAKE_MODULES}/base.cmake")
  message(STATUS "JRL cmakemodules found in 'cmake/' git submodule")
else()
  find_package(jrl-cmakemodules QUIET CONFIG)
  if(jrl-cmakemodules_FOUND)
    get_property(
      JRL_CMAKE_MODULES
      TARGET jrl-cmakemodules::jrl-cmakemodules
      PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "JRL cmakemodules found on system at ${JRL_CMAKE_MODULES}")
  elseif(${CMAKE_VERSION} VERSION_LESS "3.14.0")
    message(
      FATAL_ERROR
        "\nCan't find jrl-cmakemodules. Please either:\n"
        "  - use git submodule: 'git submodule update --init'\n"
        "  - or install https://github.com/jrl-umi3218/jrl-cmakemodules\n"
        "  - or upgrade your CMake version to >= 3.14 to allow automatic fetching\n"
    )
  else()
    message(STATUS "JRL cmakemodules not found. Let's fetch it.")
    include(FetchContent)
    FetchContent_Declare(
      "jrl-cmakemodules"
      GIT_REPOSITORY "https://github.com/jrl-umi3218/jrl-cmakemodules.git")
    FetchContent_MakeAvailable("jrl-cmakemodules")
    FetchContent_GetProperties("jrl-cmakemodules" SOURCE_DIR JRL_CMAKE_MODULES)
  endif()
endif()

include("${JRL_CMAKE_MODULES}/base.cmake")
include("${JRL_CMAKE_MODULES}/boost.cmake")
include("${JRL_CMAKE_MODULES}/ide.cmake")
include("${JRL_CMAKE_MODULES}/apple.cmake")
include("${JRL_CMAKE_MODULES}/python.cmake")
include("${JRL_CMAKE_MODULES}/cxx-standard.cmake")
include(cmake-module/qt.cmake)
include(cmake-module/osgqt.cmake)

compute_project_args(PROJECT_ARGS LANGUAGES CXX)
project(${PROJECT_NAME} ${PROJECT_ARGS})

check_minimal_cxx_standard(11 ENFORCE)

option(ENABLE_QT_DEBUG "Enable Qt debug output." FALSE)

if(APPLE)
  apply_default_apple_configuration()
endif(APPLE)

# Declare Headers
set(${PROJECT_NAME}_HEADERS_MOC
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
    ${CMAKE_SOURCE_DIR}/include/gepetto/gui/windows-manager.hh)

set(${PROJECT_NAME}_HEADERS_NO_MOC
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
    ${CMAKE_SOURCE_DIR}/include/gepetto/viewer/properties.h)

set(${PROJECT_NAME}_RESOURCES ${CMAKE_SOURCE_DIR}/res/images.qrc)

install(
  FILES fonts/arial.ttf
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/fonts
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE)

add_required_dependency("openscenegraph >= 3.2")
add_required_dependency("openthreads >= 2.6")
add_osgqt_dependency()
# Must be after ADD_OSGQT_DEPENDENCY
add_qt_dependency()
add_optional_dependency("qgv")
find_package(qgv QUIET)

# {{{ PythonQt settings
findpython()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-module/ ${CMAKE_MODULE_PATH})
# If GEPETTO_GUI_HAS_PYTHONQT is set to TRUE, then PythonQt is required.
if(DEFINED GEPETTO_GUI_HAS_PYTHONQT AND GEPETTO_GUI_HAS_PYTHONQT)
  find_package(
    PythonQt REQUIRED
    COMPONENTS QtAll
    REQUIRED)
else()
  find_package(PythonQt COMPONENTS QtAll)
endif()
# Declare option GEPETTO_GUI_HAS_PYTHONQT If GEPETTO_GUI_HAS_PYTHONQT was
# defined, this does not change its value.
if(PythonQt_FOUND AND PythonQt_QtAll_FOUND)
  set(GEPETTO_GUI_HAS_PYTHONQT
      TRUE
      CACHE BOOL "Use PythonQt dependency")
else()
  set(GEPETTO_GUI_HAS_PYTHONQT
      FALSE
      CACHE BOOL "Use PythonQt dependency")
endif()

if(GEPETTO_GUI_HAS_PYTHONQT)
  set(${PROJECT_NAME}_HEADERS_MOC
      ${${PROJECT_NAME}_HEADERS_MOC}
      ${CMAKE_SOURCE_DIR}/include/gepetto/gui/pythonwidget.hh)
endif(GEPETTO_GUI_HAS_PYTHONQT)
# }}}

find_package(
  Boost REQUIRED
  COMPONENTS
  COMPONENTS unit_test_framework)
search_for_boost_python(REQUIRED)

# Check if we are on a platform with ffmpeg or avconv
find_program(FFMPEG ffmpeg)
if(NOT FFMPEG)
  find_program(AVCONV avconv)
  if(AVCONV)
    set(FFMPEG "${AVCONV}")
  else(AVCONV)
    message(WARNING "neither ffmpeg nor avconv have been found.")
    # Fallback to old behavior: ffmpeg / avconv non-availability at configure
    # time doesn't mean anything at run time.
    set(FFMPEG "ffmpeg")
  endif(FFMPEG)
endif(NOT FFMPEG)

configure_file(${CMAKE_SOURCE_DIR}/include/gepetto/gui/config-dep.hh.in
               ${CMAKE_BINARY_DIR}/include/gepetto/gui/config-dep.hh @ONLY)

set(${PROJECT_NAME}_HEADERS ${${PROJECT_NAME}_HEADERS_MOC}
                            ${${PROJECT_NAME}_HEADERS_NO_MOC})

add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(pyplugins)
add_subdirectory(tests)

# {{{ Packaging
set(GEPETTO_VIEWER_CMAKE_PLUGIN
    "${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME}/${PROJECT_NAME}-plugin.cmake"
)
set(PKG_CONFIG_EXTRA
    "${PKG_CONFIG_EXTRA}\ncmake_plugin=${GEPETTO_VIEWER_CMAKE_PLUGIN}")
set(PACKAGE_EXTRA_MACROS
    "${PACKAGE_EXTRA_MACROS}\nset(GEPETTO_VIEWER_CMAKE_PLUGIN ${GEPETTO_VIEWER_CMAKE_PLUGIN})"
)
set(PACKAGE_EXTRA_MACROS
    "${PACKAGE_EXTRA_MACROS}\nINCLUDE(${GEPETTO_VIEWER_CMAKE_PLUGIN})")
set(PACKAGE_EXTRA_MACROS
    "${PACKAGE_EXTRA_MACROS}\nset(GEPETTO_VIEWER_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR})"
)
install(
  FILES cmake-module/${PROJECT_NAME}-plugin.cmake
  DESTINATION lib/cmake/${PROJECT_NAME}
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE)

pkg_config_append_libs(${PROJECT_NAME})
# }}}

install(FILES package.xml DESTINATION share/${PROJECT_NAME})
# vim: foldmethod=marker foldlevel=0
