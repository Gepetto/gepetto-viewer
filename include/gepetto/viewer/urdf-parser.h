//
//  urdf-parser.h
//  gepetto-viewer
//
//  Created by Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_URDFPARSER_HH
#define GEPETTO_VIEWER_URDFPARSER_HH

#include <gepetto/viewer/group-node.h>
#include <gepetto/viewer/leaf-node-collada.h>

namespace gepetto {
namespace viewer {
  namespace urdfParser {

    /// If input starts with "package://", read ROS_PACKAGE_PATH environment
    /// variable and return a suitable file, if possible.
    /// Returns input on failure (no "package://" or no file found in the packages).
    std::string getFilename (const std::string& input);

    /// Create a node from an urdf file
    /// \param robotName Name of the node that will contain the robot geometry,
    ///                  each geometric part is prefixed by this name,
    /// \param urdf_file two possible inputs:
    ///                  - if it ends with ".urdf", it is interpreted as a filename,
    ///                    that will be processed with getFilename
    ///                  - otherwise, it is interpreted as a XML string.
    /// \param visual whether to parse the visual part or the
    ///        collision part of links.
    /// \param linkFrame in the urdf kinematic chain, objects are
    ///        rigidly attached to a link. This parameter determines whether
    ///        the node frame corresponds to the link frame (if True) or
    ///        to the object frame (If False).
    /// \note the parser will replace "package://" by a path from the
    ///       ROS_PACKAGE_PATH environment variable.
    GroupNodePtr_t parse (const std::string& robotName,
			  const std::string& urdf_file,
			  const bool& visual = true,
			  const bool& linkFrame = true);
  }
} /* namespace viewer */
} /* namespace gepetto */

#endif // GEPETTO_VIEWER_URDFPARSER_HH
