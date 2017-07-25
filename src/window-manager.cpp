//
//  window-manager.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/window-manager.h>

#include <stdexcept>

#include <osg/DisplaySettings>
#include <osg/Camera>
#include <gepetto/viewer/OSGManipulator/keyboard-manipulator.h>
#include <osgGA/TrackballManipulator>
#include <osgGA/NodeTrackerManipulator>
#include <osgDB/WriteFile>

namespace graphics {
  namespace {
    struct ScreenShot : public osg::Camera::DrawCallback
    {
      ScreenShot(osgViewer::Viewer* v, const std::string& fn);

      ~ScreenShot ();

      void capture(const std::string& fn);

      virtual void operator () (osg::RenderInfo& renderInfo) const;

      std::string fn_;
      osgViewer::Viewer* viewer_;
      osg::Camera* camera_;
      unsigned int slaveIdx_;
    };

    ScreenShot::ScreenShot(osgViewer::Viewer* v, const std::string& fn)
      : fn_ (fn), viewer_ (v)
    {
      osg::Camera* main = v->getCamera();
      osg::Viewport* viewport = main->getViewport();
      osg::TraitsRefPtr traits_ptr = new ::osg::GraphicsContext::Traits();

      // traits_ptr->x = (int)viewport->x();
      // traits_ptr->y = (int)viewport->y();
      traits_ptr->x = 0;
      traits_ptr->y = 0;
      traits_ptr->width =  2*(int)viewport->width();
      traits_ptr->height = 2*(int)viewport->height();
      // traits_ptr->red = 8;
      // traits_ptr->green = 8;
      // traits_ptr->blue = 8;
      // traits_ptr->alpha = 8;
      // traits_ptr->depth = 32;
      traits_ptr->windowDecoration = false;
      traits_ptr->useCursor = false;

      traits_ptr->sharedContext = 0;

      traits_ptr->doubleBuffer = true;
      traits_ptr->pbuffer = true;
      traits_ptr->samples = 32;
      traits_ptr->sampleBuffers = 1;

      // traits_ptr->vsync = true;

      osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext( traits_ptr );

      if (!gc.valid()) {
        throw std::runtime_error("Could not create a valid GraphicsContext. Maybe your computer is not powerfull enough.");
      }

      camera_ = new ::osg::Camera;
      camera_->setGraphicsContext(gc);
      // capture_cam->setClearColor( bgColor );
      // capture_cam->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      // capture_cam->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
      // capture_cam->setRenderOrder( osg::Camera::PRE_RENDER );
      camera_->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER );
      // capture_cam->setRenderOrder(osg::Camera::POST_RENDER);
      // capture_cam->setAllowEventFocus(false);
      camera_->setViewport(new osg::Viewport(0, 0, traits_ptr->width, traits_ptr->height));
      camera_->setViewMatrix(main->getViewMatrix());

      GLenum buffer = traits_ptr->doubleBuffer ? GL_BACK : GL_FRONT;
      camera_->setDrawBuffer(buffer);
      camera_->setReadBuffer(buffer);
      // osg::ref_ptr<osg::Image> image = new osg::Image;
      // capture_cam->detach(osg::Camera::COLOR_BUFFER);
      // capture_cam->attach(osg::Camera::COLOR_BUFFER, image.get(), 0, 0);

      viewer_->addSlave(camera_, osg::Matrixd(), osg::Matrixd());
      viewer_->realize();
      slaveIdx_ = viewer_->findSlaveIndexForCamera(camera_);
      camera_->setFinalDrawCallback(this);
    }

    ScreenShot::~ScreenShot ()
    {
      if (camera_)
        viewer_->removeSlave(slaveIdx_);
    }

    void ScreenShot::operator () (osg::RenderInfo& renderInfo) const
    {
      // unsigned int frameNumber = renderInfo.getState()->getFrameStamp()->getFrameNumber();

      osg::Camera* camera = renderInfo.getCurrentCamera();
      if (camera != camera_) {
        std::cerr << "Wrong camera" << std::endl;
        return;
      }
      osg::Viewport* viewport = camera ? camera->getViewport() : 0;
      if (viewport)
      {
        osg::Timer_t tick_start = osg::Timer::instance()->tick();
        // OSG_NOTICE<<"Doing read of ="<<viewport->x()<<", "<<viewport->y()<<", "<<viewport->width()<<", "<<viewport->height()<<" with pixelFormat=0x"<<std::hex<<GL_RGBA<<std::dec<<std::endl;

        glReadBuffer(camera->getDrawBuffer());
        osg::ref_ptr<osg::Image> image = new osg::Image;

        // image->readPixels(viewport->x(),viewport->y(),viewport->width(),viewport->height(),
        image->readPixels(viewport->x(),viewport->y(),viewport->width(),viewport->height(),
            GL_RGBA, GL_UNSIGNED_BYTE, 1);

        osgDB::writeImageFile(*image, fn_);

        osg::Timer_t tick_end = osg::Timer::instance()->tick();
        std::cout<<"Time to generate image = "<<osg::Timer::instance()->delta_s(tick_start, tick_end)<<" seconds"<<std::endl;
      }
      camera->setFinalDrawCallback(0);
      std::cout << "called" << std::endl;
    }
  }

    void WindowManager::createManipulator()
    {
      osgViewer::Viewer::Windows windows;
      viewer_ptr_->getWindows(windows);
      manipulator_ptr = new ::osgGA::KeySwitchMatrixManipulator;
      manipulator_ptr->addMatrixManipulator('1',"trackball",new ::osgGA::TrackballManipulator);
      manipulator_ptr->addMatrixManipulator('2',"keyboard",new ::osgGA::KeyboardManipulator(windows.front()));
      manipulator_ptr->addMatrixManipulator('3',"tracker",new ::osgGA::NodeTrackerManipulator);
      viewer_ptr_->setCameraManipulator( manipulator_ptr);      
    }
  
  void WindowManager::createBackground()
  {
    main_camera_->setClearMask(GL_DEPTH_BUFFER_BIT);
    main_camera_->setClearColor(osg::Vec4(0.,0.,0.,0.));
    // Create background camera
    bg_camera_ = new osg::Camera;
    bg_camera_->setName("bg_camera");
    const osg::Node::NodeMask mask = ~IntersectionBit;
    bg_camera_->setNodeMask(mask);
    
    osg::ref_ptr <const osg::GraphicsContext::Traits> traits_ptr = gc_->getTraits();
    
    // set the projection matrix
    bg_camera_->setProjectionMatrix(osg::Matrix::ortho2D(traits_ptr->x,traits_ptr->width,traits_ptr->y,traits_ptr->height));
    
    bg_camera_->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    bg_camera_->setViewMatrix(osg::Matrix::identity());
    
    // only clear the depth buffer
    //      bg_camera_->setClearMask(GL_DEPTH_BUFFER_BIT);
    bg_camera_->setClearColor(osg::Vec4(0.,0.,0.,0.));
    
    // draw subgraph before main camera view.
    bg_camera_->setRenderOrder(osg::Camera::PRE_RENDER);
    
    bg_camera_->setAllowEventFocus(false);
    
    bg_color1_ = osg::Vec4(0.4f, 0.4f, 0.6f, 1.0f);
    bg_color2_ = osg::Vec4(0.6f, 0.6f, 0.6f, 1.0f);
    
    // Create the background geometry here
    {
      osg::Geode* geode = new osg::Geode();
      geode->setName("background");
      osg::StateSet* stateset = geode->getOrCreateStateSet();
      stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
      
      bg_geom_ = new osg::Geometry;
      
      const float depth = (float)-0.1;
      osg::Vec3Array* vertices = new osg::Vec3Array;
      vertices->push_back(osg::Vec3(0.,(float)traits_ptr->height,depth));
      vertices->push_back(osg::Vec3(0.,0.,depth));
      vertices->push_back(osg::Vec3((float)traits_ptr->width,0.,depth));
      vertices->push_back(osg::Vec3((float)traits_ptr->width,(float)traits_ptr->height,depth));
      bg_geom_->setVertexArray(vertices);
      
      osg::Vec3Array* normals = new osg::Vec3Array;
      normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
      bg_geom_->setNormalArray(normals, osg::Array::BIND_OVERALL);
      
      applyBackgroundColor();
      
      bg_geom_->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0,4));
      
      stateset = bg_geom_->getOrCreateStateSet();
      stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
      stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
      
      geode->addDrawable(bg_geom_);
      bg_camera_->addChild(geode);
      //bg_camera_->setGraphicsContext(camera->getGraphicsContext());
    }
    
    scene_ptr_->asGroup()->addChild(bg_camera_);
  }
  
  void WindowManager::applyBackgroundColor()
  {
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(bg_color1_);
    colors->push_back(bg_color2_);
    colors->push_back(bg_color2_);
    colors->push_back(bg_color1_);
    bg_geom_->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
  }

  void WindowManager::captureFrame(const std::string& filename)
  {
    osg::ref_ptr<ScreenShot> screenshot_ = new ScreenShot(viewer_ptr_, filename);
    viewer_ptr_->frame();
    std::cout << "captured" << std::endl;
  }

    /* Declaration of private function members */
    void WindowManager::init(const unsigned int& x,
                                  const unsigned int& y,
                                  const unsigned int& width,
                                  const unsigned int& height)
    {
      osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
      osg::TraitsRefPtr traits_ptr = new ::osg::GraphicsContext::Traits(ds);

      traits_ptr->windowName = "Gepetto Viewer";
      traits_ptr->x = x;
      traits_ptr->y = y;
      traits_ptr->width = width;
      traits_ptr->height = height;
      traits_ptr->windowDecoration = true;
      traits_ptr->doubleBuffer = true;
      traits_ptr->sharedContext = 0;

      traits_ptr->vsync = true;
      traits_ptr->readDISPLAY ();
      traits_ptr->setUndefinedScreenDetailsToDefaultScreen ();

      osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext( traits_ptr );

      init (gc.get ());
    }

    void WindowManager::init(osg::GraphicsContext* gc)
    {
      const std::string name = "scene";
      scene_ptr_ = ::graphics::GroupNode::create(name);

      viewer_ptr_ = new ::osgViewer::Viewer();

      /* init main camera */
      main_camera_ = viewer_ptr_->getCamera ();
      main_camera_->setName("main_camera");

      gc_ = osg::GraphicsContextRefPtr (gc);
      const osg::GraphicsContext::Traits* traits_ptr = gc->getTraits ();
      main_camera_->setGraphicsContext(gc);
      main_camera_->setViewport(new osg::Viewport(0,0, traits_ptr->width, traits_ptr->height));
      main_camera_->setProjectionMatrixAsPerspective(
          30.0f, static_cast<double>(traits_ptr->width)/static_cast<double>(traits_ptr->height), 1.0f, 10000.0f );
      GLenum buffer = traits_ptr->doubleBuffer ? GL_BACK : GL_FRONT;
      main_camera_->setDrawBuffer(buffer);
      main_camera_->setReadBuffer(buffer);

      /* add scene to the viewer */
      viewer_ptr_->setSceneData ( scene_ptr_->asGroup() );
      
      /* avoid ending */
      viewer_ptr_->setKeyEventSetsDone (0);
      viewer_ptr_->addEventHandler(new osgViewer::HelpHandler);
      createBackground();
      createManipulator();
    }

    void WindowManager::init(osgViewer::Viewer* v, osg::GraphicsContext* gc)
    {
      std::string name = "scene";
      scene_ptr_ = ::graphics::GroupNode::create(name);
      
      viewer_ptr_ = v;
      viewer_ptr_->setSceneData ( scene_ptr_->asGroup() );
      
      /* init main camera */
      main_camera_ = viewer_ptr_->getCamera ();
      
      gc_ = osg::GraphicsContextRefPtr (gc);
      createBackground();
      createManipulator();
    }

    WindowManager::WindowManager () : nodeTrackerManipulatorIndex(2)
    {
        init (0, 0, DEF_WIDTH_WINDOW, DEF_HEIGHT_WINDOW);
    }

    WindowManager::WindowManager (osg::GraphicsContext* gc) : nodeTrackerManipulatorIndex(2)
    {
        init (gc);
    }

    WindowManager::WindowManager (osgViewer::Viewer* v, osg::GraphicsContext* gc) : nodeTrackerManipulatorIndex(2)
    {
        init (v, gc);
    }

    WindowManager::WindowManager (const unsigned int& x,
                                            const unsigned int& y,
                                            const unsigned int& width,
                                            const unsigned int& height) : nodeTrackerManipulatorIndex(2)
    {
        init (x, y, width, height);
    }

    WindowManager::WindowManager (const WindowManager& other) : nodeTrackerManipulatorIndex(2)
    {
      init ((unsigned int) other.getWindowPosition().x(),
	    (unsigned int) other.getWindowPosition().y(),
	    (unsigned int) other.getWindowDimension().x(),
	    (unsigned int) other.getWindowDimension().y());
    }

    void WindowManager::initWeakPtr (WindowManagerWeakPtr other_weak_ptr)
    {
        weak_ptr_ = other_weak_ptr;
    }

    /* End declaration of private function members */

    /* Declaration of protected function members */

    WindowManagerPtr_t WindowManager::create ()
    {
        WindowManagerPtr_t shared_ptr(new WindowManager());

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    WindowManagerPtr_t WindowManager::create (const unsigned int& x,
                                                            const unsigned int& y,
                                                            const unsigned int& width,
                                                            const unsigned int& height)
    {
        WindowManagerPtr_t shared_ptr(new WindowManager(x, y, width, height));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    WindowManagerPtr_t WindowManager::create(osg::GraphicsContext* gc)
    {
        WindowManagerPtr_t shared_ptr(new WindowManager(gc));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    WindowManagerPtr_t WindowManager::create(osgViewer::Viewer* v, osg::GraphicsContext* gc)
    {
        WindowManagerPtr_t shared_ptr(new WindowManager(v, gc));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    WindowManagerPtr_t WindowManager::createCopy (WindowManagerPtr_t other)
    {
        WindowManagerPtr_t shared_ptr(new WindowManager(*other));

        // Add reference to itself
        shared_ptr->initWeakPtr(shared_ptr);

        return shared_ptr;
    }

    /* End declaration of protected function members */

    /* Declaration of public function members */

    WindowManagerPtr_t WindowManager::clone (void) const
    {
        return WindowManager::createCopy(weak_ptr_.lock());
    }

    WindowManagerPtr_t WindowManager::self (void) const
    {
        return weak_ptr_.lock();
    }

    bool WindowManager::addNode(NodePtr_t graphical_object_ptr)
    {
        bool result = scene_ptr_->addChild (graphical_object_ptr);
        return result;
    }

    bool WindowManager::done ()
    {
        return  viewer_ptr_->done();
    }

    bool WindowManager::frame ()
    {
        viewer_ptr_->frame();
        return true;
    }

    bool WindowManager::run ()
    {
        return viewer_ptr_->run();
    }

    void WindowManager::setWindowDimension (const unsigned int& width,
                                                 const unsigned int& height)
    {
        /* Define new trait dimension of the main camera */
        const osg::GraphicsContext::Traits* traits_ptr = gc_->getTraits ();
        gc_->resized (traits_ptr->x, traits_ptr->y, width, height);
    }

    osgVector2 WindowManager::getWindowDimension() const
    {
        osgVector2 dimention;
        const osg::GraphicsContext::Traits* traits_ptr = gc_->getTraits ();
        dimention.x() = (osg::Vec2f::value_type) traits_ptr->width;
        dimention.y() = (osg::Vec2f::value_type) traits_ptr->height;
        return dimention;
    }

    void WindowManager::setWindowPosition (const unsigned int& x_position,
                                                const unsigned int& y_position)
    {
        /* Define new trait dimension of the main camera */
        const osg::GraphicsContext::Traits* traits_ptr = gc_->getTraits ();
        gc_->resized (x_position, y_position,
                traits_ptr->width, traits_ptr->height);
    }

    osgVector2 WindowManager::getWindowPosition() const
    {
        osgVector2 position;
        const osg::GraphicsContext::Traits* traits_ptr = gc_->getTraits ();
        position.x() = (osg::Vec2f::value_type) traits_ptr->x;
        position.y() = (osg::Vec2f::value_type) traits_ptr->y;
        return position;
    }

    WindowManager::~WindowManager()
    {
      stopCapture ();
      scene_ptr_.reset();
      viewer_ptr_ = NULL;
    }
  
  osgViewer::ViewerRefPtr WindowManager::getViewerClone()
  {
    return ::osgViewer::ViewerRefPtr(viewer_ptr_.get());
  }
  
  void WindowManager::startCapture (const std::string& filename,
      const std::string& extension)
  {
    if (screen_capture_ptr_.valid ()) {
      screen_capture_ptr_->startCapture ();
      return;
    }
    /* Create an handler to save video */
    write_to_file_ptr_ = new osgViewer::ScreenCaptureHandler::WriteToFile
      (filename, extension);
    screen_capture_ptr_ = new osgViewer::ScreenCaptureHandler
      (write_to_file_ptr_.get (), -1);
    /* Screen capture can be stopped with stopCapture */
    screen_capture_ptr_->setKeyEventTakeScreenShot (0);
    screen_capture_ptr_->setKeyEventToggleContinuousCapture (0);
    viewer_ptr_->addEventHandler (screen_capture_ptr_);
    screen_capture_ptr_->startCapture ();
  }

  void WindowManager::stopCapture ()
  {
    if (!screen_capture_ptr_) return;
    screen_capture_ptr_->stopCapture ();
    frame ();
  }

  bool WindowManager::writeNodeFile (const std::string& fn)
  {
    osg::ref_ptr <osgDB::Options> options = new osgDB::Options;
    options->setOptionString ("NoExtras");
    return osgDB::writeNodeFile (*(viewer_ptr_->getSceneData()), fn,
        options.get());
  }

  void WindowManager::attachCameraToNode(NodePtr_t node)
  {
    osgGA::NodeTrackerManipulator* manip
      = dynamic_cast<osgGA::NodeTrackerManipulator*>
      (manipulator_ptr->getMatrixManipulatorWithIndex(nodeTrackerManipulatorIndex));

    if (manip)
      {
	manip->setTrackNode(node->getOsgNode().get());
	// manip->setRotationMode(osgGA::NodeTrackerManipulator::TRACKBALL);
	manip->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
	manipulator_ptr->selectMatrixManipulator(nodeTrackerManipulatorIndex);
      }
    else
      std::cout << "Unexpected manipulator in viewer" << std::endl;
  }

  void WindowManager::detachCamera()
  {
    manipulator_ptr->selectMatrixManipulator(0);
  }

  void WindowManager::getCameraTransform(osg::Vec3d& pos,osg::Quat& rot){
    osg::Matrixd matrix = manipulator_ptr->getMatrix();
    matrix.get(rot);
    pos = matrix.getTrans();
  }

  void WindowManager::setCameraTransform(const osg::Vec3d &pos, const osg::Quat &rot){
   osg::Matrixd matrix= osg::Matrixd();
   matrix.setTrans(pos.x(),pos.y(),pos.z());
   matrix.setRotate(rot);
   manipulator_ptr->setByMatrix(matrix);
  }

  
    /* End declaration of public function members */

} /* namespace graphics */
