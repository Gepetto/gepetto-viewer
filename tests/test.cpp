//
//  test.cpp
//  Test for SceneViwer
//
//  Created by Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <iostream>
#include <gepetto/viewer/window-manager.h>
#include <gepetto/viewer/node.h>
#include <gepetto/viewer/group-node.h>
#include <gepetto/viewer/leaf-node-box.h>
#include <gepetto/viewer/leaf-node-capsule.h>
#include <gepetto/viewer/leaf-node-cone.h>
#include <gepetto/viewer/leaf-node-cylinder.h>
#include <gepetto/viewer/leaf-node-line.h>
#include <gepetto/viewer/leaf-node-face.h>
#include <gepetto/viewer/leaf-node-sphere.h>
#include <gepetto/viewer/leaf-node-ground.h>
#include <gepetto/viewer/leaf-node-collada.h>
#include <gepetto/viewer/urdf-parser.h>

  int main(int, const char**)

  {
    using namespace graphics;
osg::ref_ptr< graphics::Robot > modelNode=    graphics:: urdfParser::parse(std::string("hrp2"),
    std::string("/home/xeul/SRC/Pinocchio/nao_robot-master/nao_description/urdf/naoV50_generated_urdf/nao.urdf"),
    std::string("/home/xeul/SRC/Pinocchio/nao_robot-master/"));

  osgViewer::Viewer viewer;
    viewer.setUpViewInWindow( 10, 30, 800, 600 );
    viewer.setSceneData( modelNode );
  viewer.realize();
  viewer.run();
  }
