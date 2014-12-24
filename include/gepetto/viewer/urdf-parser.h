//
//  urdf-parser.h
//  gepetto-viewer
//
//  Created by Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_URDFPARSER_HH
#define SCENEVIEWER_URDFPARSER_HH

#include <urdf_model/model.h>
#include <urdf_parser/urdf_parser.h>
#include <gepetto/viewer/group-node.h>
#include <gepetto/viewer/leaf-node-collada.h>

namespace graphics {
  namespace urdfParser {

    /// Create a node from an urdf file
    /// \param robotName Name of the node that will contain the robot geometry,
    ///                  each geometric part is prefixed by this name,
    /// \param urdf_file_path to the package containing the urdf file,
    ///                       i.e. "/opt/ros/hydro/share/pr2_description",
    /// \param meshDataRootDir path to the package that contains the collada
    ///                        files,
    ///                        i.e. "/opt/ros/hydro/share/"
    /// \param collisionOrVisual whether to parse the visual part or the
    ///        collision part of links.
    /// \note the parser will replace "package://" by meshDataRootDir in the
    ///       urdf file.
    GroupNodePtr_t parse (const std::string& robotName,
			  const std::string& urdf_file_path,
			  const std::string& meshDataRootDir,
			  const std::string& collisionOrVisual = "visual");

  }
}

#endif // SCENEVIEWER_URDFPARSER_HH
