//
//  window-manager.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/window-manager.h>

#include <osg/Camera>
#include <gepetto/viewer/OSGManipulator/keyboard-manipulator.h>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgDB/WriteFile>

namespace graphics {

    /* Declaration of private function members */
    void WindowManager::init(const unsigned int& x,
                                  const unsigned int& y,
                                  const unsigned int& width,
                                  const unsigned int& height)
    {
      osg::TraitsRefPtr traits_ptr = new ::osg::GraphicsContext::Traits;

      traits_ptr->windowName = "Gepetto Viewer";
      traits_ptr->x = x;
      traits_ptr->y = y;
      traits_ptr->width = width;
      traits_ptr->height = height;
      traits_ptr->windowDecoration = true;
      traits_ptr->doubleBuffer = true;
      traits_ptr->sharedContext = 0;

#ifndef DISABLE_SAMPLING
      traits_ptr->sampleBuffers = 1;
      traits_ptr->samples = 1;
#else
      traits_ptr->sampleBuffers = 0;
      traits_ptr->samples = 0;
#endif
      traits_ptr->vsync = true;
      traits_ptr->readDISPLAY ();
      traits_ptr->setUndefinedScreenDetailsToDefaultScreen ();

      osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext( traits_ptr );

      init (gc.get ());
    }

    void WindowManager::init(osg::GraphicsContext* gc)
    {
      std::string name = "root";
      scene_ptr_ = ::graphics::GroupNode::create(name);

      viewer_ptr_ = new ::osgViewer::Viewer();

      /* init main camera */
      main_camera_ = viewer_ptr_->getCamera ();

      gc_ = osg::GraphicsContextRefPtr (gc);
      const osg::GraphicsContext::Traits* traits_ptr = gc->getTraits ();
      main_camera_->setGraphicsContext(gc);
      main_camera_->setViewport(new osg::Viewport(0,0, traits_ptr->width, traits_ptr->height));
      main_camera_->setProjectionMatrixAsPerspective(
          30.0f, static_cast<double>(traits_ptr->width)/static_cast<double>(traits_ptr->height), 1.0f, 10000.0f );
      GLenum buffer = traits_ptr->doubleBuffer ? GL_BACK : GL_FRONT;
      main_camera_->setDrawBuffer(buffer);
      main_camera_->setReadBuffer(buffer);

      /* add camera to the viewer */
      viewer_ptr_->setSceneData ( scene_ptr_->asGroup() );
      viewer_ptr_->setKeyEventSetsDone (0);

      osgViewer::Viewer::Windows windows;
      viewer_ptr_->getWindows(windows);
      osgGA::KeySwitchMatrixManipulator *manipulator_ptr = new ::osgGA::KeySwitchMatrixManipulator;
      manipulator_ptr->addNumberedMatrixManipulator(new ::osgGA::TrackballManipulator);
      manipulator_ptr->addNumberedMatrixManipulator(new ::osgGA::KeyboardManipulator(windows.front()));
      manipulator_ptr->addNumberedMatrixManipulator(new ::osgGA::FirstPersonManipulator);
      viewer_ptr_->setCameraManipulator( manipulator_ptr);
    }

    void WindowManager::init(osgViewer::Viewer* v, osg::GraphicsContext* gc)
    {
        std::string name = "root";
        scene_ptr_ = ::graphics::GroupNode::create(name);

        viewer_ptr_ = v;
        viewer_ptr_->setSceneData ( scene_ptr_->asGroup() );

        /* init main camera */
        main_camera_ = viewer_ptr_->getCamera ();

        gc_ = osg::GraphicsContextRefPtr (gc);
    }

    WindowManager::WindowManager ()
    {
        init (0, 0, DEF_WIDTH_WINDOW, DEF_HEIGHT_WINDOW);
    }

    WindowManager::WindowManager (osg::GraphicsContext* gc)
    {
        init (gc);
    }

    WindowManager::WindowManager (osgViewer::Viewer* v, osg::GraphicsContext* gc)
    {
        init (v, gc);
    }

    WindowManager::WindowManager (const unsigned int& x,
                                            const unsigned int& y,
                                            const unsigned int& width,
                                            const unsigned int& height)
    {
        init (x, y, width, height);
    }

    WindowManager::WindowManager (const WindowManager& other)
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
        viewer_ptr_->home(); // Partie du code à revoir pour inclure juste un nouveau focus

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

    /* End declaration of public function members */

} /* namespace graphics */
