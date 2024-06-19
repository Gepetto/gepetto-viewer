# Copyright (c) 2018 CNRS Authors: Joseph Mirabel
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

if(PROJECT_USE_QT4)
  set(QT4 "QT4")
else()
  set(QT4 "")
endif()

include_directories(SYSTEM "${PYTHON_INCLUDE_DIR}" "${PYTHONQT_INCLUDE_DIR}")

gepetto_gui_plugin(
  pyqgv
  ${QT4}
  HEADERS_NO_MOC
  plugin.hh
  decorator.hh
  SOURCES
  plugin.cc
  decorator.cc
  LINK_DEPENDENCIES
  ${PYTHONQT_LIBRARIES}
  ${PROJECT_NAME}
  qgv::qgvcore)
