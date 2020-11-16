#
# Copyright (c) 2014 CNRS
# Authors: Mathieu Geisert
#
#
# This file is part of gepetto-viewer
# gepetto-viewer is free software: you can redistribute it
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

SET(LIBRARY_NAME ${PROJECT_NAME})

# {{{ libgepetto-viewer.so

# {{{ Files
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/internal/configuration.cc.in
  ${CMAKE_CURRENT_BINARY_DIR}/internal/configuration.cc
  @ONLY)

SET(${LIBRARY_NAME}_SOURCES
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
  gui/point-intersector.cc
)

SET(HEADERS)
FOREACH(header ${${PROJECT_NAME}_HEADERS})
  LIST(APPEND HEADERS ${PROJECT_SOURCE_DIR}/${header})
ENDFOREACH()
ADD_HEADER_GROUP(HEADERS)
ADD_SOURCE_GROUP(${LIBRARY_NAME}_SOURCES)
# }}}

# {{{ Configure PythonQt
IF(GEPETTO_GUI_HAS_PYTHONQT)
  INCLUDE_DIRECTORIES(SYSTEM "${PYTHON_INCLUDE_DIR}" "${PYTHONQT_INCLUDE_DIR}")
  SET (${PROJECT_NAME}_SOURCES
    ${${PROJECT_NAME}_SOURCES}
    gui/pythonwidget.cc
    gui/python-bindings.cc
    gui/python-decorator.hh)
  SET (${PROJECT_NAME}_MOC
    ${${PROJECT_NAME}_HEADERS_MOC}
    gui/python-decorator.hh
    )
ELSE (GEPETTO_GUI_HAS_PYTHONQT)
  MESSAGE(STATUS "Skipping PythonQt settings")
  SET (${PROJECT_NAME}_MOC
    ${${PROJECT_NAME}_HEADERS_MOC}
    )
ENDIF(GEPETTO_GUI_HAS_PYTHONQT)
# }}}

# {{{ Configure Qt
IF(PROJECT_USE_QT4)
  QT4_WRAP_CPP(${PROJECT_NAME}_HEADERS_MOCED ${${PROJECT_NAME}_MOC})
  #QT4_WRAP_UI(${PROJECT_NAME}_FORMS_HEADERS ${${PROJECT_NAME}_FORMS})
  QT4_ADD_RESOURCES(${PROJECT_NAME}_RESOURCES_RCC ${${PROJECT_NAME}_RESOURCES})
  INCLUDE(${QT_USE_FILE})
ELSE(PROJECT_USE_QT4)
  QT5_WRAP_CPP(${PROJECT_NAME}_HEADERS_MOCED ${${PROJECT_NAME}_MOC})
  #QT5_WRAP_UI(${PROJECT_NAME}_FORMS_HEADERS ${${PROJECT_NAME}_FORMS})
  QT5_ADD_RESOURCES(${PROJECT_NAME}_RESOURCES_RCC ${${PROJECT_NAME}_RESOURCES})
ENDIF(PROJECT_USE_QT4)
# }}}

# {{{ Compile and link
ADD_LIBRARY(${LIBRARY_NAME} SHARED
  ${${PROJECT_NAME}_HEADERS_MOCED}
  ${${PROJECT_NAME}_HEADERS_NO_MOC}
  ${${PROJECT_NAME}_RESOURCES_RCC}
  ${${LIBRARY_NAME}_SOURCES})
IF(NOT ${ENABLE_QT_DEBUG})
  TARGET_COMPILE_OPTIONS(${LIBRARY_NAME}
    PRIVATE -DQT_NO_DEBUG_OUTPUT -DQT_NO_INFO_OUTPUT -DQT_NO_WARNING_OUTPUT)
ENDIF(NOT ${ENABLE_QT_DEBUG})
IF(GEPETTO_GUI_HAS_PYTHONQT)
  TARGET_LINK_LIBRARIES(${LIBRARY_NAME} PUBLIC ${PYTHON_LIBRARY})
ENDIF(GEPETTO_GUI_HAS_PYTHONQT)

PKG_CONFIG_USE_DEPENDENCY(${LIBRARY_NAME} openscenegraph)
PKG_CONFIG_USE_DEPENDENCY(${LIBRARY_NAME} ${OSGQT_DEPENDENCY})
PKG_CONFIG_USE_DEPENDENCY(${LIBRARY_NAME} openthreads)

TARGET_COMPILE_DEFINITIONS(${LIBRARY_NAME}
  PRIVATE -DAVCONV="${AVCONV}"
  PUBLIC ${QT_DEFINITIONS})
TARGET_LINK_LIBRARIES(${LIBRARY_NAME}
  PUBLIC
    ${PROJECT_QT_LIBS}
    ${PYTHONQT_LIBRARIES}
)
TARGET_INCLUDE_DIRECTORIES(${LIBRARY_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  ${Boost_INCLUDE_DIR}
  $<INSTALL_INTERFACE:include>)
IF(GEPETTO_GUI_HAS_PYTHONQT)
  TARGET_LINK_BOOST_PYTHON(${LIBRARY_NAME})
ENDIF(GEPETTO_GUI_HAS_PYTHONQT)

INSTALL(TARGETS ${LIBRARY_NAME}
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION lib)
# }}}
# }}}

# {{{ gepetto-gui
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/gui/main.cc.in
  ${CMAKE_CURRENT_BINARY_DIR}/gui/main.cc
  @ONLY)
ADD_EXECUTABLE(gepetto-gui ${CMAKE_CURRENT_BINARY_DIR}/gui/main.cc)
TARGET_COMPILE_DEFINITIONS(gepetto-gui
  PUBLIC ${QT_DEFINITIONS})
PKG_CONFIG_USE_DEPENDENCY(gepetto-gui openscenegraph)
TARGET_LINK_LIBRARIES(gepetto-gui PUBLIC ${LIBRARY_NAME})

INSTALL(TARGETS gepetto-gui
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION bin)
# }}}

# {{{ gvtools
ADD_EXECUTABLE(gvtools gvtools.cpp)
PKG_CONFIG_USE_DEPENDENCY(gvtools openscenegraph)

INSTALL(TARGETS gvtools DESTINATION bin)
# }}}

IF(${CMAKE_VERSION} VERSION_LESS 3.0)
    SET(UIC_CMAKE_28_COMPAT_DEPENDENCIES)
    SET(${LIBRARY_NAME}_DIALOG_UIC
        pluginmanagerdialog
        dialogloadrobot
        dialogloadenvironment
        )
    SET(${LIBRARY_NAME}_UIC
        mainwindow
        )
    FOREACH(ui ${${LIBRARY_NAME}_DIALOG_UIC})
        # don't try to support Qt5 if CMake < 3.0
        QT4_WRAP_UI("ui_${ui}.h" "gui/dialog/${ui}.ui")
        SET(UIC_CMAKE_28_COMPAT_DEPENDENCIES ${UIC_CMAKE_28_COMPAT_DEPENDENCIES} "ui_${ui}.h")
    ENDFOREACH(ui ${${LIBRARY_NAME}_DIALOG_UIC})
    FOREACH(ui ${${LIBRARY_NAME}_UIC})
        QT4_WRAP_UI("ui_${ui}.h" "gui/${ui}.ui")
        SET(UIC_CMAKE_28_COMPAT_DEPENDENCIES ${UIC_CMAKE_28_COMPAT_DEPENDENCIES} "ui_${ui}.h")
    ENDFOREACH(ui ${${LIBRARY_NAME}_UIC})
    ADD_CUSTOM_TARGET(UIC_CMAKE_28_COMPAT ALL DEPENDS ${UIC_CMAKE_28_COMPAT_DEPENDENCIES})
    ADD_DEPENDENCIES(${LIBRARY_NAME} UIC_CMAKE_28_COMPAT)
ENDIF(${CMAKE_VERSION} VERSION_LESS 3.0)


# vim: foldmethod=marker foldlevel=0
