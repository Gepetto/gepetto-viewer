# Copyright (c) 2016 LAAS-CNRS
# Author: Joseph Mirabel
#
# This file is part of gepetto-viewer-corba.
# gepetto-viewer-corba is free software: you can redistribute it
# and/or modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation, either version
# 3 of the License, or (at your option) any later version.
#
# gepetto-viewer-corba is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Lesser Public License for more details.  You should have
# received a copy of the GNU Lesser General Public License along with
# hpp-corbaserver.  If not, see
# <http://www.gnu.org/licenses/>.

IF(GEPETTO_GUI_HAS_PYTHONQT)
  INSTALL(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gepetto/gui/blenderexport.py
    ${CMAKE_CURRENT_SOURCE_DIR}/gepetto/gui/intersection.py
    ${CMAKE_CURRENT_SOURCE_DIR}/gepetto/gui/__init__.py
    DESTINATION ${PYTHON_SITELIB}/gepetto/gui
    )
  GEPETTO_GUI_PYPLUGIN(gepetto/gui/intersection.py)
  GEPETTO_GUI_PYPLUGIN(gepetto/gui/blenderexport.py)
ENDIF(GEPETTO_GUI_HAS_PYTHONQT)
