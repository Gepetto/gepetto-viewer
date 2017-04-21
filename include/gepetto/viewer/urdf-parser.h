//
//  urdf-parser.h
//  gepetto-viewer
//
//  Created by Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_URDFPARSER_HH
#define SCENEVIEWER_URDFPARSER_HH

#include <gepetto/viewer/group-node.h>
#include <gepetto/viewer/leaf-node-collada.h>

namespace graphics {
  namespace urdfParser {

    /// If input starts with "package://", read ROS_PACKAGE_PATH environment
    /// variable and return a suitable file, if possible.
    /// Returns input on failure (no "package://" or no file found in the packages).
    std::string getFilename (const std::string& input);

    /// Create a node from an urdf file
    /// \param robotName Name of the node that will contain the robot geometry,
    ///                  each geometric part is prefixed by this name,
    /// \param urdf_file_path to the package containing the urdf file,
    ///                       i.e. "/opt/ros/hydro/share/pr2_description",
    /// \param visual whether to parse the visual part or the
    ///        collision part of links.
    /// \param linkFrame in the urdf kinematic chain, objects are
    ///        rigidly attached to a link. This parameter determines whether
    ///        the node frame corresponds to the link frame (if True) or
    ///        to the object frame (If False).
    /// \note the parser will replace "package://" by a path from the
    ///       ROS_PACKAGE_PATH environment variable.
    GroupNodePtr_t parse (const std::string& robotName,
			  const std::string& urdf_file_path,
			  const bool& visual = true,
			  const bool& linkFrame = true);
  }
}

#endif // SCENEVIEWER_URDFPARSER_HH
