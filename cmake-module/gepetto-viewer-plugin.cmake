# Copyright (c) 2018 CNRS Authors: Joseph Mirabel
#
# This file is part of gepetto-viewer-corba gepetto-viewer-corba is free
# software: you can redistribute it and/or modify it under the terms of the GNU
# Lesser General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# gepetto-viewer-corba is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Lesser Public License
# for more details.  You should have received a copy of the GNU Lesser General
# Public License along with gepetto-viewer-corba  If not, see
# <http://www.gnu.org/licenses/>.

# See plugins/pluginskeleton/CMakeLists.txt for description of how to use this
# macro.
macro(GEPETTO_GUI_PLUGIN PLUGIN_NAME)

  set(options QT4)
  set(oneValueArgs)
  set(multiValueArgs
      MOC_OPTIONS
      HEADERS
      HEADERS_NO_MOC
      FORMS
      RESOURCES
      SOURCES
      LINK_DEPENDENCIES
      PKG_CONFIG_DEPENDENCIES)
  cmake_parse_arguments(PLUGIN "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN})

  if(DEFINED GEPETTO_VIEWER_INCLUDEDIR)
    set(MOC_INCLUDE_GEPETTO_VIEWER "-I${GEPETTO_VIEWER_INCLUDEDIR}"
                                   ${PLUGIN_MOC_OPTIONS})
  endif(DEFINED GEPETTO_VIEWER_INCLUDEDIR)

  if(PLUGIN_QT4)
    qt4_wrap_cpp(PLUGIN_HEADERS_MOC ${PLUGIN_HEADERS} OPTIONS
                 ${MOC_INCLUDE_GEPETTO_VIEWER})
    qt4_wrap_ui(PLUGIN_FORMS_HEADERS ${PLUGIN_FORMS})
    qt4_add_resources(PLUGIN_RESOURCES_RCC ${PLUGIN_RESOURCES})
    include(${QT_USE_FILE})
  else()
    qt5_wrap_cpp(PLUGIN_HEADERS_MOC ${PLUGIN_HEADERS} OPTIONS
                 ${MOC_INCLUDE_GEPETTO_VIEWER})
    qt5_wrap_ui(PLUGIN_FORMS_HEADERS ${PLUGIN_FORMS})
    qt5_add_resources(PLUGIN_RESOURCES_RCC ${PLUGIN_RESOURCES})
  endif()

  if(APPLE)
    set(CMAKE_MACOSX_RPATH TRUE)
  endif(APPLE)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/gepetto-gui-plugins")

  add_library(
    ${PLUGIN_NAME} SHARED
    ${PLUGIN_SOURCES} ${PLUGIN_HEADERS_NO_MOC} ${PLUGIN_HEADERS_MOC}
    ${PLUGIN_FORMS_HEADERS} ${PLUGIN_RESOURCES_RCC})
  target_include_directories(${PLUGIN_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_compile_definitions(
    ${PLUGIN_NAME} PRIVATE ${QT_DEFINITIONS} -DQT_PLUGIN -DQT_NO_DEBUG
                           -DQT_SHARED)

  set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
  if(APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES SUFFIX ".so")
  endif(APPLE)

  target_link_libraries(${PLUGIN_NAME} PUBLIC ${QT_LIBRARIES}
                                              ${PLUGIN_LINK_DEPENDENCIES})
  foreach(DEP ${PLUGIN_PKG_CONFIG_DEPENDENCIES})
    pkg_config_use_dependency(${PLUGIN_NAME} ${DEP})
  endforeach()

  # Uncomment to install
  install(TARGETS ${PLUGIN_NAME} DESTINATION lib/gepetto-gui-plugins)
endmacro()

# Declare a Python plugin
macro(GEPETTO_GUI_PYPLUGIN file)
  string(MD5 file_md5 "${PYTHON_SITELIB}/${file}")
  string(REGEX REPLACE ".py$" "" module "${file}")
  string(REPLACE "/" "." module "${module}")
  file(
    GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file_md5}
    CONTENT "${file}\n${module}")

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${file_md5}
          DESTINATION etc/gepetto-gui/pyplugin.conf.d)
endmacro()
