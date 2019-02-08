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

ADD_EXECUTABLE(testing test.cpp)
TARGET_LINK_LIBRARIES(testing ${PROJECT_NAME})

ADD_UNIT_TEST(nodes nodes)
TARGET_LINK_LIBRARIES(nodes ${PROJECT_NAME} ${Boost_LIBRARIES})
