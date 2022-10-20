#
# Copyright (c) 2014 CNRS Authors: Mathieu Geisert
#
# This file is part of gepetto-viewer gepetto-viewer is free software: you can
# redistribute it and/or modify it under the terms of the GNU Lesser General
# Public License as published by the Free Software Foundation, either version 3
# of the License, or (at your option) any later version.
#
# gepetto-viewer is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Lesser Public License for more
# details.  You should have received a copy of the GNU Lesser General Public
# License along with gepetto-viewer  If not, see <http://www.gnu.org/licenses/>.

set(LIBRARY_NAME ${PROJECT_NAME})

# {{{ libgepetto-viewer.so

# {{{ Files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/internal/configuration.cc.in
               ${CMAKE_CURRENT_BINARY_DIR}/internal/configuration.cc @ONLY)

set(${LIBRARY_NAME}_SOURCES
    group-node.cpp
    node.cpp
    node-drawable.cpp
    node-property.cpp
    window-manager.cpp
    windows-manager.cpp
    leaf-node-line.cpp
    leaf-node-box.cpp
    leaf-node-cylinder.cpp
    leaf-node-cone.cpp
    leaf-node-face.cpp
    leaf-node-sphere.cpp
    leaf-node-capsule.cpp
    leaf-node-ground.cpp
    leaf-node-collada.cpp
    leaf-node-light.cpp
    leaf-node-mesh.cpp
    urdf-parser.cpp
    leaf-node-xyzaxis.cpp
    leaf-node-arrow.cpp
    roadmap-viewer.cpp
    node-rod.cpp
    node-visitor.cc
    transform-writer.cc
    blender-geom-writer.cc
    OSGManipulator/keyboard-manipulator.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/internal/configuration.cc
    properties.cpp
    log.hh
    log.cc
    gui/action-search-bar.cc
    gui/bodytreewidget.cc
    gui/dialog/configuration.cc
    gui/dialog/dialogloadenvironment.cc
    gui/dialog/dialogloadrobot.cc
    gui/dialog/pluginmanagerdialog.cc
    gui/ledindicator.cc
    gui/mainwindow.cc
    gui/node-action.cc
    gui/osgwidget.cc
    gui/pick-handler.cc
    gui/selection-event.cc
    gui/selection-handler.cc
    gui/settings.cc
    gui/shortcut-factory.cc
    gui/tree-item.cc
    gui/windows-manager.cc
    gui/point-intersector.cc)

set(HEADERS)
foreach(header ${${PROJECT_NAME}_HEADERS})
  list(APPEND HEADERS ${PROJECT_SOURCE_DIR}/${header})
endforeach()
add_header_group(HEADERS)
add_source_group(${LIBRARY_NAME}_SOURCES)
# }}}

# {{{ Configure PythonQt
if(GEPETTO_GUI_HAS_PYTHONQT)
  include_directories(SYSTEM "${PYTHON_INCLUDE_DIR}" "${PYTHONQT_INCLUDE_DIR}")
  set(${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_SOURCES} gui/pythonwidget.cc
                              gui/python-bindings.cc gui/python-decorator.hh)
  set(${PROJECT_NAME}_MOC ${${PROJECT_NAME}_HEADERS_MOC}
                          gui/python-decorator.hh)
else(GEPETTO_GUI_HAS_PYTHONQT)
  message(STATUS "Skipping PythonQt settings")
  set(${PROJECT_NAME}_MOC ${${PROJECT_NAME}_HEADERS_MOC})
endif(GEPETTO_GUI_HAS_PYTHONQT)
# }}}

# {{{ Configure Qt
if(PROJECT_USE_QT4)
  qt4_wrap_cpp(${PROJECT_NAME}_HEADERS_MOCED ${${PROJECT_NAME}_MOC})
  # QT4_WRAP_UI(${PROJECT_NAME}_FORMS_HEADERS ${${PROJECT_NAME}_FORMS})
  qt4_add_resources(${PROJECT_NAME}_RESOURCES_RCC ${${PROJECT_NAME}_RESOURCES})
  include(${QT_USE_FILE})
else(PROJECT_USE_QT4)
  qt5_wrap_cpp(${PROJECT_NAME}_HEADERS_MOCED ${${PROJECT_NAME}_MOC})
  # QT5_WRAP_UI(${PROJECT_NAME}_FORMS_HEADERS ${${PROJECT_NAME}_FORMS})
  qt5_add_resources(${PROJECT_NAME}_RESOURCES_RCC ${${PROJECT_NAME}_RESOURCES})
endif(PROJECT_USE_QT4)
# }}}

# {{{ Compile and link
add_library(
  ${LIBRARY_NAME} SHARED
  ${${PROJECT_NAME}_HEADERS_MOCED} ${${PROJECT_NAME}_HEADERS_NO_MOC}
  ${${PROJECT_NAME}_RESOURCES_RCC} ${${LIBRARY_NAME}_SOURCES})
if(NOT ${ENABLE_QT_DEBUG})
  target_compile_options(
    ${LIBRARY_NAME} PRIVATE -DQT_NO_DEBUG_OUTPUT -DQT_NO_INFO_OUTPUT
                            -DQT_NO_WARNING_OUTPUT)
endif(NOT ${ENABLE_QT_DEBUG})
if(GEPETTO_GUI_HAS_PYTHONQT)
  target_link_libraries(${LIBRARY_NAME} PUBLIC ${PYTHON_LIBRARY})
endif(GEPETTO_GUI_HAS_PYTHONQT)

pkg_config_use_dependency(${LIBRARY_NAME} openscenegraph)
pkg_config_use_dependency(${LIBRARY_NAME} ${OSGQT_DEPENDENCY})
pkg_config_use_dependency(${LIBRARY_NAME} openthreads)

target_compile_definitions(
  ${LIBRARY_NAME}
  PRIVATE -DFFMPEG="${FFMPEG}"
  PUBLIC ${QT_DEFINITIONS})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${PROJECT_QT_LIBS}
                                             ${PYTHONQT_LIBRARIES})
target_include_directories(
  ${LIBRARY_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> ${Boost_INCLUDE_DIR}
         ${OPENSCENEGRAPH_INCLUDE_DIRS} $<INSTALL_INTERFACE:include>)
if(GEPETTO_GUI_HAS_PYTHONQT)
  target_link_boost_python(${LIBRARY_NAME})
endif(GEPETTO_GUI_HAS_PYTHONQT)

install(
  TARGETS ${LIBRARY_NAME}
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION lib)
# }}} }}}

# {{{ gepetto-gui
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gui/main.cc.in
               ${CMAKE_CURRENT_BINARY_DIR}/gui/main.cc @ONLY)
add_executable(gepetto-gui ${CMAKE_CURRENT_BINARY_DIR}/gui/main.cc)
target_compile_definitions(gepetto-gui PUBLIC ${QT_DEFINITIONS})
pkg_config_use_dependency(gepetto-gui openscenegraph)
target_link_libraries(gepetto-gui PUBLIC ${LIBRARY_NAME})

install(
  TARGETS gepetto-gui
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION bin)
# }}}

# {{{ gvtools
add_executable(gvtools gvtools.cpp)
pkg_config_use_dependency(gvtools openscenegraph)

install(TARGETS gvtools DESTINATION bin)
# }}}

if(${CMAKE_VERSION} VERSION_LESS 3.0)
  set(UIC_CMAKE_28_COMPAT_DEPENDENCIES)
  set(${LIBRARY_NAME}_DIALOG_UIC pluginmanagerdialog dialogloadrobot
                                 dialogloadenvironment)
  set(${LIBRARY_NAME}_UIC mainwindow)
  foreach(ui ${${LIBRARY_NAME}_DIALOG_UIC})
    # don't try to support Qt5 if CMake < 3.0
    qt4_wrap_ui("ui_${ui}.h" "gui/dialog/${ui}.ui")
    set(UIC_CMAKE_28_COMPAT_DEPENDENCIES ${UIC_CMAKE_28_COMPAT_DEPENDENCIES}
                                         "ui_${ui}.h")
  endforeach(ui ${${LIBRARY_NAME}_DIALOG_UIC})
  foreach(ui ${${LIBRARY_NAME}_UIC})
    qt4_wrap_ui("ui_${ui}.h" "gui/${ui}.ui")
    set(UIC_CMAKE_28_COMPAT_DEPENDENCIES ${UIC_CMAKE_28_COMPAT_DEPENDENCIES}
                                         "ui_${ui}.h")
  endforeach(ui ${${LIBRARY_NAME}_UIC})
  add_custom_target(UIC_CMAKE_28_COMPAT ALL
                    DEPENDS ${UIC_CMAKE_28_COMPAT_DEPENDENCIES})
  add_dependencies(${LIBRARY_NAME} UIC_CMAKE_28_COMPAT)
endif(${CMAKE_VERSION} VERSION_LESS 3.0)

# vim: foldmethod=marker foldlevel=0
