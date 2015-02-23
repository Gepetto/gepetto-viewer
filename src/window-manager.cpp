//
//  window-manager.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/window-manager.h>

#include <osg/Camera>
#include <osgGA/TrackballManipulator>

namespace graphics {

    /* Declaration of private function members */
    void WindowManager::init(const unsigned int& x,
                                  const unsigned int& y,
                                  const unsigned int& width,
                                  const unsigned int& height)
    {
        std::string name = "root";
        scene_ptr_ = ::graphics::GroupNode::create(name);

        viewer_ptr_ = new ::osgViewer::Viewer();
        viewer_ptr_->setSceneData ( scene_ptr_->asGroup() );

        viewer_ptr_->setCameraManipulator( new ::osgGA::TrackballManipulator );

        /* init main camera */
        main_camera_ = new ::osg::Camera;

        traits_ptr_ = new ::osg::GraphicsContext::Traits;

        traits_ptr_->windowName = "Gepetto Viewer";
        traits_ptr_->x = x;
        traits_ptr_->y = y;
        traits_ptr_->width = width;
        traits_ptr_->height = height;
        traits_ptr_->windowDecoration = true;
        traits_ptr_->doubleBuffer = true;
        traits_ptr_->sharedContext = 0;
        traits_ptr_->sampleBuffers = 1;
        traits_ptr_->samples = 1;
        traits_ptr_->readDISPLAY ();
        traits_ptr_->setUndefinedScreenDetailsToDefaultScreen ();

        osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext( traits_ptr_ );

        main_camera_->setGraphicsContext(gc.get());
        main_camera_->setViewport(new osg::Viewport(0,0, traits_ptr_->width, traits_ptr_->height));
        GLenum buffer = traits_ptr_->doubleBuffer ? GL_BACK : GL_FRONT;
        main_camera_->setDrawBuffer(buffer);
        main_camera_->setReadBuffer(buffer);

        /* add camera to the viewer */
        viewer_ptr_->addSlave ( main_camera_ );

        screen_capture_ptr_ = NULL;
        write_to_file_ptr_ = NULL;
    }

    WindowManager::WindowManager () :
      screen_capture_ptr_ (NULL), write_to_file_ptr_ (NULL)
    {
        init (0, 0, DEF_WIDTH_WINDOW, DEF_HEIGHT_WINDOW);
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
        traits_ptr_->width = width;
        traits_ptr_->height = height;

        main_camera_->setViewport (traits_ptr_->x, traits_ptr_->y, width, height);
    }

    osgVector2 WindowManager::getWindowDimension() const
    {
        osgVector2 dimention;
        dimention.x() = (osg::Vec2f::value_type) width_window_dimension_;
        dimention.y() = (osg::Vec2f::value_type) height_window_dimension_;
        return dimention;
    }

    void WindowManager::setWindowPosition (const unsigned int& x_position,
                                                const unsigned int& y_position)
    {
        /* Define new trait dimension of the main camera */
        traits_ptr_->x = x_position;
        traits_ptr_->y = y_position;

        //main_camera_->setViewport (traits_ptr_->x, traits_ptr_->y, width, height);
    }

    osgVector2 WindowManager::getWindowPosition() const
    {
        osgVector2 position;
        position.x() = (osg::Vec2f::value_type) x_window_position_;
        position.y() = (osg::Vec2f::value_type) y_window_position_;
        return position;
    }


    void WindowManager::setWindowDecoration (bool window_decoration_status)
    {
        traits_ptr_->windowDecoration = window_decoration_status;
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
    if (screen_capture_ptr_ != NULL) {
      screen_capture_ptr_->startCapture ();
      return;
    }
    /* Create an handler to save video */
    write_to_file_ptr_ = new osgViewer::ScreenCaptureHandler::WriteToFile
      (filename, extension);
    screen_capture_ptr_ = new osgViewer::ScreenCaptureHandler
      (write_to_file_ptr_, -1);
    /* Screen capture can be stopped with stopCapture */
    screen_capture_ptr_->setKeyEventTakeScreenShot (0);
    screen_capture_ptr_->setKeyEventToggleContinuousCapture (0);
    viewer_ptr_->addEventHandler (screen_capture_ptr_);
    screen_capture_ptr_->startCapture ();
  }

  void WindowManager::stopCapture ()
  {
    if (screen_capture_ptr_ == NULL) return;
    screen_capture_ptr_->stopCapture ();
    frame ();
  }

    /* End declaration of public function members */

} /* namespace graphics */
