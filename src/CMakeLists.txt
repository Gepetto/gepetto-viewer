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

CONFIG_FILES(
  internal/configuration.cc
)

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
)

SET(HEADERS)
FOREACH(header ${${PROJECT_NAME}_HEADERS})
  LIST(APPEND HEADERS ${PROJECT_SOURCE_DIR}/${header})
ENDFOREACH()

ADD_LIBRARY(${LIBRARY_NAME} SHARED ${HEADERS} ${${LIBRARY_NAME}_SOURCES})

ADD_HEADER_GROUP(HEADERS)
ADD_SOURCE_GROUP(${LIBRARY_NAME}_SOURCES)

PKG_CONFIG_USE_DEPENDENCY(${LIBRARY_NAME} openscenegraph)
TARGET_LINK_LIBRARIES(${LIBRARY_NAME}
  ${Boost_LIBRARIES}
  ${X11_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${PROJECT_QT_LIBS})

INSTALL(TARGETS ${LIBRARY_NAME} DESTINATION lib)

ADD_EXECUTABLE(gvtools gvtools.cpp)
PKG_CONFIG_USE_DEPENDENCY(gvtools openscenegraph)
TARGET_LINK_LIBRARIES(gvtools ${Boost_LIBRARIES} ${OPENGL_LIBRARIES})

INSTALL(TARGETS gvtools DESTINATION bin)
