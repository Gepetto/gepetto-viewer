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

#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>


class myKeyboardEventHandler : public osgGA::GUIEventHandler
{
public:
    graphics:: RobotUpdate *_control;
    myKeyboardEventHandler(graphics::RobotUpdate *controller):_control(controller) {}
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);
    virtual void accept(osgGA::GUIEventHandlerVisitor& v)
    {
        v.visit(*this);
    };
};

bool myKeyboardEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    switch(ea.getEventType())
    {
    case(osgGA::GUIEventAdapter::KEYDOWN):
    {
        switch(ea.getKey())
        {
        case 'w':
            std::cout << " w key pressed" << std::endl;
            _control->addOrder(new graphics::SwitchDebugOrder(*_control->getRobot()));
            return false;
            break;
        default:
            return false;
        }
    }
    default:
        return false;
    }
}
int main(int, const char**)

{
    using namespace graphics;
    osg::ref_ptr< graphics::Robot > modelNode=    graphics:: urdfParser::parse(std::string("hrp2"),
            std::string("/home/xeul/SRC/Pinocchio/nao_robot-master/nao_description/urdf/naoV50_generated_urdf/nao.urdf"),
            std::string("/home/xeul/SRC/Pinocchio/nao_robot-master/"));
    osg::ref_ptr< RobotUpdate > controller= new RobotUpdate(modelNode);
    modelNode->addUpdateCallback(controller);

    osgViewer::Viewer viewer;
    viewer.addEventHandler(new myKeyboardEventHandler(controller));
    // add the thread model handler
    viewer.addEventHandler(new osgViewer::ThreadingHandler);

    // add the window size toggle handler
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);

    // add the stats handler
    viewer.addEventHandler(new osgViewer::StatsHandler);

    // add the help handler
    //viewer.addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));

    // add the record camera path handler
    viewer.addEventHandler(new osgViewer::RecordCameraPathHandler);

    // add the LOD Scale handler
    viewer.addEventHandler(new osgViewer::LODScaleHandler);

    // add the screen capture handler
    viewer.addEventHandler(new osgViewer::ScreenCaptureHandler);
    viewer.setUpViewInWindow( 10, 30, 800, 600 );
    viewer.setSceneData( modelNode );
    viewer.realize();
    viewer.run();
}
