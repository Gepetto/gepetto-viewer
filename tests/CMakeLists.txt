#
# Copyright (c) 2019 CNRS Authors: Joseph Mirabel
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

macro(ADD_TEST_CFLAGS target flag)
  set_property(
    TARGET ${target}
    APPEND_STRING
    PROPERTY COMPILE_FLAGS " ${flag}")
endmacro(ADD_TEST_CFLAGS)

add_executable(testing test.cpp)
target_link_libraries(testing PRIVATE ${PROJECT_NAME})
pkg_config_use_dependency(testing openscenegraph)

add_unit_test(nodes nodes.cpp)
add_test_cflags(nodes "-DBOOST_TEST_DYN_LINK")
target_link_libraries(nodes PRIVATE ${PROJECT_NAME})
pkg_config_use_dependency(nodes openscenegraph)
target_link_libraries(nodes PRIVATE Boost::unit_test_framework)
add_executable(test-gl gl.cpp)
target_link_libraries(test-gl PRIVATE ${PROJECT_NAME})
pkg_config_use_dependency(test-gl openscenegraph)

if($ENV{DISPLAY})
  add_unit_test(properties properties.cpp)
  add_test_cflags(properties "-DBOOST_TEST_DYN_LINK")
  target_link_libraries(properties PRIVATE ${PROJECT_NAME}
                                           Boost::unit_test_framework)

  add_unit_test(safe-application safe-application.cpp)
  add_test_cflags(safe-application "-DBOOST_TEST_DYN_LINK")
  target_link_libraries(safe-application PRIVATE ${PROJECT_NAME})
  target_link_libraries(safe-application PRIVATE Boost::unit_test_framework)
endif($ENV{DISPLAY})
