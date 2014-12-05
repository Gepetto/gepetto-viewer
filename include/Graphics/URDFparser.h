//
//  URDFparser.h
//  SceneViewer
//
//  Created by Anthony Couret, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_URDFPARSER_HH
#define SCENEVIEWER_URDFPARSER_HH

#include <urdf_model/model.h>
#include <urdf_parser/urdf_parser.h>
#include "GroupNode.h"
#include "LeafNodeCollada.h"

namespace Graphics {
namespace URDFparser{

        GroupNodePtr_t parse(const std::string& robotName, const std::string& urdf_file_path, const std::string& package_path);

}    
}

#endif // SCENEVIEWER_URDFPARSER_HH
