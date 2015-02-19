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

FILE(TO_CMAKE_PATH ${CMAKE_HOME_DIRECTORY}/cmake PROJ_MODULE_PATH)
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJ_MODULE_PATH})

SET(CXX_DISABLE_WERROR TRUE)
INCLUDE(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/eigen.cmake)
INCLUDE(cmake/cpack.cmake)
INCLUDE(cmake/python.cmake)

SET(PROJECT_NAME gepetto)
SET(PROJECT_DESCRIPTION "Robot viewer")
SET(PROJECT_URL "")

# Where to compile shared objects
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SETUP_PROJECT()

IF(WIN32)
  SET(LINK copy_if_different)
ELSE(WIN32)
  SET(LINK create_symlink)
ENDIF(WIN32)
SET(PROJECT_DESCRIPTION
  "Implement simple Viewer using OpenSceneGraph."
  )



set(PINOCCHIO_PATH "${CMAKE_HOME_DIRECTORY}/../pinocchio" CACHE FILEPATH "${CMAKE_HOME_DIRECTORY}/../pinocchio")
# Declare Headers
SET(${PROJECT_NAME}_HEADERS
  #include/gepetto/viewer/config-osg.h
  #include/gepetto/viewer/group-node.h
  #include/gepetto/viewer/leaf-node-box.h
 # include/gepetto/viewer/leaf-node-capsule.h
  #include/gepetto/viewer/leaf-node-collada.h
  #include/gepetto/viewer/leaf-node-cone.h
 # include/gepetto/viewer/leaf-node-cylinder.h
  #include/gepetto/viewer/leaf-node-face.h
  #include/gepetto/viewer/leaf-node-ground.h
 #include/gepetto/viewer/leaf-node-line.h
 # include/gepetto/viewer/leaf-node-sphere.h
 # include/gepetto/viewer/macros.h
 # include/gepetto/viewer/node.h
include/gepetto/viewer/robot.h
include/gepetto/viewer/viewer.h
  include/gepetto/viewer/urdf-parser.h
  include/gepetto/viewer/window-manager.h
  include/gepetto/viewer/window-manager.h
  include/gepetto/python/python.hpp
include/gepetto/python/graphicrobot.hpp
include/gepetto/python/graphicviewer.hpp
)

 SET(HEADERSPYTHON
  #include/gepetto/viewer/config-osg.h
  #include/gepetto/viewer/group-node.h
  #include/gepetto/viewer/leaf-node-box.h
 # include/gepetto/viewer/leaf-node-capsule.h
  #include/gepetto/viewer/leaf-node-collada.h
  #include/gepetto/viewer/leaf-node-cone.h
 # include/gepetto/viewer/leaf-node-cylinder.h
  #include/gepetto/viewer/leaf-node-face.h
  #include/gepetto/viewer/leaf-node-ground.h
 #include/gepetto/viewer/leaf-node-line.h
 # include/gepetto/viewer/leaf-node-sphere.h
 # include/gepetto/viewer/macros.h
 # include/gepetto/viewer/node.h
include/gepetto/viewer/robot.h
  include/gepetto/viewer/urdf-parser.h
  include/gepetto/viewer/window-manager.h
)
MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/include/${PROJECT_NAME}/python")

FOREACH(header ${${PROJECT_NAME}_HEADERS})
  GET_FILENAME_COMPONENT(headerName ${header} NAME)
  GET_FILENAME_COMPONENT(headerPath ${header} PATH)
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E ${LINK}
    ${${PROJECT_NAME}_SOURCE_DIR}/include/${header}
    ${${PROJECT_NAME}_BINARY_DIR}/include/${PROJECT_NAME}/${header})
  INSTALL(FILES ${${PROJECT_NAME}_SOURCE_DIR}/${header}
	  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}/${headerPath}
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE)
ENDFOREACH(header)

find_package(OpenSceneGraph REQUIRED)
find_package(Eigen3 REQUIRED)
#ADD_REQUIRED_DEPENDENCY("openscenegraph >= 3.2")
#ADD_REQUIRED_DEPENDENCY("openthreads >= 3.2")
ADD_OPTIONAL_DEPENDENCY("eigenpy >= 1.2.0")
ADD_REQUIRED_DEPENDENCY("urdfdom")

ADD_SUBDIRECTORY(src)

PKG_CONFIG_APPEND_LIBS(${PROJECT_NAME})

IF(EIGENPY_FOUND)
  FINDPYTHON()
  MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/lib/python/${PROJECT_NAME}")

  # --- COMPILE WRAPPER
  SET(PYWRAP ${PROJECT_NAME}_pywrap)
  ADD_LIBRARY(${PYWRAP} SHARED src/python/module.cpp src/python/python.cpp)
  PKG_CONFIG_USE_DEPENDENCY(${PYWRAP} eigenpy)
  IF(URDFDOM_FOUND)
    PKG_CONFIG_USE_DEPENDENCY(${PYWRAP} urdfdom)

  ENDIF(URDFDOM_FOUND)
  TARGET_LINK_LIBRARIES(${PYWRAP} ${Boost_LIBRARIES} eigenpy ${OPENTHREADS_LIBRARIES}
   ${OPENSCENEGRAPH_LIBRARIES} ${OSGVIEWER_LIBRARY} ${OSGDB_LIBRARY} ${OSGGA_LIBRARY} ${PROJECT_NAME})
  SET_TARGET_PROPERTIES(${PYWRAP} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/python/${PROJECT_NAME}")
  INSTALL(FILES
    "${CMAKE_BINARY_DIR}/lib/python/${PROJECT_NAME}/lib${PYWRAP}.so"
    DESTINATION ${PYTHON_SITELIB}/${PROJECT_NAME})

  # --- INSTALL SCRIPTS
  SET(PYTHON_FILES
    python/__init__.py
    #python/utils.py
   # python/robot_wrapper.py
   # python/rpy.py
   # python/explog.py
    )
  FOREACH(python ${PYTHON_FILES})
    GET_FILENAME_COMPONENT(pythonFile ${python} NAME)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E ${LINK}
      ${${PROJECT_NAME}_SOURCE_DIR}/src/${python}
      ${${PROJECT_NAME}_BINARY_DIR}/lib/python/${PROJECT_NAME}/${pythonFile})

    # Tag pyc file as generated.
    SET_SOURCE_FILES_PROPERTIES(
      "${CMAKE_CURRENT_BINARY_DIR}/lib/python/${PROJECT_NAME}/${pythonFile}c"
      PROPERTIES GENERATED TRUE)

    EXECUTE_PROCESS(COMMAND
      ${PYTHON_EXECUTABLE} -m py_compile
      ${CMAKE_CURRENT_BINARY_DIR}/lib/python/${PROJECT_NAME}/${pythonFile} )

    # Clean generated files.
    SET_PROPERTY(
      DIRECTORY APPEND PROPERTY
      ADDITIONAL_MAKE_CLEAN_FILES
      "${CMAKE_CURRENT_BINARY_DIR}/lib/python/${PROJECT_NAME}/${pythonFile}c")

    INSTALL(FILES
      "${CMAKE_CURRENT_BINARY_DIR}/lib/python/${PROJECT_NAME}/${pythonFile}"
      "${CMAKE_CURRENT_BINARY_DIR}/lib/python/${PROJECT_NAME}/${pythonFile}c"
      DESTINATION ${PYTHON_SITELIB}/${PROJECT_NAME})
  ENDFOREACH(python)
ENDIF(EIGENPY_FOUND)

SETUP_PROJECT_FINALIZE()
#SETUP_PROJECT_CPACK()
