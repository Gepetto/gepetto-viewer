#
# Copyright (c) 2019 CNRS
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

MACRO(ADD_TEST_CFLAGS target flag)
  SET_PROPERTY(TARGET ${target} APPEND_STRING PROPERTY COMPILE_FLAGS " ${flag}")
ENDMACRO(ADD_TEST_CFLAGS)

ADD_EXECUTABLE(testing test.cpp)
TARGET_LINK_LIBRARIES(testing PRIVATE ${PROJECT_NAME})
PKG_CONFIG_USE_DEPENDENCY(testing openscenegraph)

ADD_UNIT_TEST(nodes nodes.cpp)
ADD_TEST_CFLAGS(nodes "-DBOOST_TEST_DYN_LINK")
TARGET_LINK_LIBRARIES(nodes PRIVATE ${PROJECT_NAME})
PKG_CONFIG_USE_DEPENDENCY(nodes openscenegraph)
TARGET_LINK_LIBRARIES(nodes PRIVATE Boost::unit_test_framework)
ADD_EXECUTABLE(test-gl gl.cpp)
TARGET_LINK_LIBRARIES(test-gl PRIVATE ${PROJECT_NAME})
PKG_CONFIG_USE_DEPENDENCY(test-gl openscenegraph)


IF($ENV{DISPLAY})
  ADD_UNIT_TEST(properties properties.cpp)
  ADD_TEST_CFLAGS(properties "-DBOOST_TEST_DYN_LINK")
  TARGET_LINK_LIBRARIES(properties PRIVATE ${PROJECT_NAME} Boost::unit_test_framework)

  ADD_UNIT_TEST(safe-application safe-application.cpp)
  ADD_TEST_CFLAGS(safe-application "-DBOOST_TEST_DYN_LINK")
  TARGET_LINK_LIBRARIES(safe-application PRIVATE ${PROJECT_NAME})
  TARGET_LINK_LIBRARIES(safe-application PRIVATE Boost::unit_test_framework)
ENDIF($ENV{DISPLAY})
