// Copyright (c) 2017, LAAS-CNRS
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of hpp-core.
// hpp-core is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// hpp-core is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// hpp-core. If not, see <http://www.gnu.org/licenses/>.

#ifndef SCENEVIEWER_INTERNAL_CONFIGURATION_HH
#define SCENEVIEWER_INTERNAL_CONFIGURATION_HH

#include <gepetto/viewer/config-osg.h>

#include <osgText/Font>

namespace graphics {
  osg::ref_ptr<osgText::Font> defaultFont();
} /* namespace graphics */

#endif // SCENEVIEWER_INTERNAL_CONFIGURATION_HH
