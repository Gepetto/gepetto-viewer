#
# Copyright (c) 2014 LAAS-CNRS
# Authors: Mathieu Geisert
#
#
# This file is part of SceneViewer.
# SceneViewer is free software: you can redistribute it
# and/or modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation, either version
# 3 of the License, or (at your option) any later version.
#
# hpp-core is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Lesser Public License for more details.  You should have
# received a copy of the GNU Lesser General Public License along with
# hpp-core  If not, see
# <http://www.gnu.org/licenses/>.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CXX_DISABLE_WERROR TRUE)
INCLUDE(cmake/base.cmake)
INCLUDE(cmake/eigen.cmake)
#INCLUDE(cmake/cpack.cmake)

SET(PROJECT_NAME SceneViewer)
SET(PROJECT_DESCRIPTION
  "Implement simple Viewer using OpenSceneGraph."
  )
SET(PROJECT_URL "")

# Where to compile shared objects
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

SETUP_PROJECT()

# Declare Headers
SET(${PROJECT_NAME}_HEADERS
  include/Graphics/config.h
  include/Graphics/GroupNode.h
  include/Graphics/LeafNodeBox.h
  include/Graphics/LeafNodeCapsule.h
  include/Graphics/LeafNodeCollada.h
  include/Graphics/LeafNodeCone.h
  include/Graphics/LeafNodeCylinder.h
  include/Graphics/LeafNodeFace.h
  include/Graphics/LeafNodeGround.h
  include/Graphics/LeafNodeLine.h
  include/Graphics/LeafNodeSphere.h
  include/Graphics/macros.h
  include/Graphics/Node.h
  include/Graphics/URDFparser.h
  include/Graphics/WindowManager.h
)

ADD_REQUIRED_DEPENDENCY("openscenegraph >= 3.2")
ADD_REQUIRED_DEPENDENCY("openthreads >= 3.2")
ADD_REQUIRED_DEPENDENCY("urdfdom")

ADD_SUBDIRECTORY(src)

SETUP_PROJECT_FINALIZE()
#SETUP_PROJECT_CPACK()
