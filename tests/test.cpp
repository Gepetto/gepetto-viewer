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

    LeafNodeBoxPtr_t box = LeafNodeBox::create("box1", osgVector3(1.,1.,1.));
    /*LeafNodeCapsulePtr_t capsule = LeafNodeCapsule::create("capsule1", 1,1);
    LeafNodeConePtr_t cone = LeafNodeCone::create("cone", 1,1);
    LeafNodeCylinderPtr_t cylindre = LeafNodeCylinder::create("cylindre", 1,1);
    LeafNodeSpherePtr_t sphere = LeafNodeSphere::create("sphere", 1);
    LeafNodeGroundPtr_t ground = LeafNodeGround::create("ground");*/
    //LeafNodeColladaPtr_t collada = LeafNodeCollada::create("collada","/home/simeval/AluminumChair.dae");
    box->addLandmark(1.);
    //LeafNodeLinePtr_t line = LeafNodeLine::create(std::string("line"), osgVector3(1.0,1.0,1.0), osgVector3(0.0,0.0,0.0));
    //LeafNodeFacePtr_t face = LeafNodeFace::create(std::string("face"), osgVector3(0.0,0.0,0.0), osgVector3(-2.0,0.0,0.0), osgVector3(-2.0,-2.0,0.0), osgVector3(0.0,-2.0,0.0));
    //face->addVertex(osgVector3(0.,0.,2.));

    GroupNodePtr_t world = GroupNode::create(std::string("world"));
    //GroupNodePtr_t robot = GroupNode::create(std::string("robot"));
    //    GroupNodePtr_t robot = urdfParser::parse(std::string("hrp2"), 
    //std::string("/home/ostasse/devel/ros-indigo-1/install/share/hrp2_14_description/urdf/hrp2_14.urdf"),
    //std::string("/home/ostasse/devel/ros-indigo-1/install/share/"));


    world->addChild(box);
    /*world->addChild(obstacle);

    DefVector3 position1(2.,0.,0.);
    DefQuat attitude1(1.,0.,0.,0.);
    Tools::ConfigurationPtr_t config1 = Tools::Configuration::create(position1, attitude1);
    DefVector3 position2(0.,2.,0.);
    DefQuat attitude2(1.,0.,0.,0.);
    Tools::ConfigurationPtr_t config2 = Tools::Configuration::create(position2, attitude2);

    robot->addChild(box);
    robot->applyConfiguration(config1);
    box->applyConfiguration(config1);
    robot->addChild(capsule);
    capsule->setVisibilityMode(VISIBILITY_OFF);
    robot->addChild(cone);
    cone->applyConfiguration(config2);
    cone->setColor(osgVector4(1.,1.,0.5,1.));
    cone->setVisibilityMode(VISIBILITY_ON);
    DefVector3 position3(0.,0.,8.);
    DefQuat attitude3(1.,0.,0.,0.);
    Tools::ConfigurationPtr_t config3 = Tools::Configuration::create(position3, attitude3);
    obstacle->applyConfiguration(config3);
    obstacle->addChild(cylindre);
    sphere->applyConfiguration(config2);
    obstacle->addChild(sphere);
    sphere->setAlpha(0.1f);
    world->addChild(ground);
    world->addChild(collada);
    collada->applyConfiguration(config2);
    std::string name("world/robot/genou");
    std::cout << (parseName(name)) << std::endl;*/

    //world->addChild(ground);
    //world->addChild(line);
    world->addChild(box);
    //world->addChild(robot);
    WindowManagerPtr_t gm = WindowManager::create();
    gm->addNode(world);
    //osgViewer::Viewer viewer;
    //viewer.setSceneData( world->asGroup() );
    box->deleteLandmark();
    box->addLandmark(2.);
    box->applyConfiguration(osgVector3(0.,0.,0.), osgQuat(0.884,0.306,-0.177,0.306));
    world->addLandmark(1.);

    return gm->run();
  }
