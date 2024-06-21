#
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

include(${CMAKE_SOURCE_DIR}/cmake-module/gepetto-viewer-plugin.cmake)

# ADD_SUBDIRECTORY(plugin-skeleton)

if(PythonQt_FOUND)
  option(BUILD_PY_QGV "Build PyQGV plugin" ${QGV_FOUND})
  option(BUILD_PY_QCUSTOM_PLOT "Build PyQCustomPlot plugin" OFF)
endif(PythonQt_FOUND)

if(PythonQt_FOUND AND BUILD_PY_QGV)
  add_subdirectory(pyqgv)
endif(PythonQt_FOUND AND BUILD_PY_QGV)

if(PythonQt_FOUND AND BUILD_PY_QCUSTOM_PLOT)
  add_subdirectory(pyqcustomplot)
endif(PythonQt_FOUND AND BUILD_PY_QCUSTOM_PLOT)
