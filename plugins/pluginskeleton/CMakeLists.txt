# Copyright (c) 2018 CNRS
# Authors: Joseph Mirabel
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

IF(PROJECT_USE_QT4)
  SET(QT4 "QT4")
ELSE()
  SET(QT4 "")
ENDIF()

## 1. Tells pkg-config to read qtversion and cmake_plugin from pkg config file.
#
#  LIST(APPEND PKG_CONFIG_ADDITIONAL_VARIABLES qtversion cmake_plugin)

## 2. Include macro GEPETTO_GUI_PLUGIN
#
# INCLUDE(${GEPETTO_VIEWER_CMAKE_PLUGIN})

## 3. Call GEPETTO_GUI_PLUGIN as follows
#
GEPETTO_GUI_PLUGIN(pluginskeleton
  # Whether to use Qt4 or Qt5
  ${QT4}

  # List of headers that need not to be moced.
  HEADERS_NO_MOC

  # List of headers to be moced
  HEADERS
  plugin.hh

  # List of Qt forms
  FORMS

  # List of Qt resources
  RESOURCES

  # List of source files
  SOURCES
  plugin.cc

  # List of dependencies to be given to TARGET_LINK_LIBRARIES
  LINK_DEPENDENCIES

  # List of dependencies to be given to PKG_CONFIG_USE_DEPENDENCY
  PKG_CONFIG_DEPENDENCIES
  )
