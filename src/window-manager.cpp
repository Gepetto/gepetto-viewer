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

#include <../src/is-dirty-visitor.h>
#include <../src/internal/configuration.hh>

namespace gepetto {
namespace viewer {
  namespace {
    struct ScreenShot : public osg::Camera::DrawCallback
    {
      ScreenShot(const std::string& fn) : fn_ (fn) {}

      virtual void operator () (osg::RenderInfo& renderInfo) const;

      std::string fn_;
    };

    void ScreenShot::operator () (osg::RenderInfo& renderInfo) const
    {
      osg::Timer* timer = osg::Timer::instance();
      osg::Timer_t tick_start = timer->tick();

      osg::ref_ptr<osg::Image> image = new osg::Image;
      assert(renderInfo.getState());
      osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();
      assert(gc);

      glReadBuffer(GL_BACK);
      //osg::Camera* camera = renderInfo.getCurrentCamera();
      //osg::Viewport* viewport = camera ? camera->getViewport() : 0;
      //image->readPixels(viewport->x(),viewport->y(),viewport->width(),viewport->height(),
      image->readPixels(0, 0, gc->getTraits()->width, gc->getTraits()->height,
          GL_RGBA, GL_UNSIGNED_BYTE, 1);

      osgDB::writeImageFile(*image, fn_);

      bool res = osgDB::writeImageFile(*image, fn_);
      if (!res) {
        std::cerr << "Failed to write image " << fn_ << std::endl;
      } else {
        osg::Timer_t tick_end = timer->tick();
        std::cout<<"Time to generate image " << fn_ << " = "
          << timer->delta_s(tick_start, tick_end) << " seconds"<<std::endl;
      }
    }

    struct ResizeHandler: osgGA::GUIEventHandler
    {
      ResizeHandler(osg::Camera* camera) : camera_ (camera) {}

      virtual bool handle(
          const osgGA::GUIEventAdapter& gea,
          osgGA::GUIActionAdapter&      /*gaa*/,
          osg::Object*                  /*obj*/,
          osg::NodeVisitor*             /*nv*/
          )
      {
        osgGA::GUIEventAdapter::EventType ev = gea.getEventType();

        if(ev != osgGA::GUIEventAdapter::RESIZE) return false;

        float w = (float)gea.getWindowWidth();
        float h = (float)gea.getWindowHeight();

        if(camera_.valid())
          camera_->setProjectionMatrix(osg::Matrix::ortho2D(0.0f, w, 0.0f, h));

        float m = w*0.01f;

        texts_[WindowManager::BOTTOM][WindowManager::LEFT  ]->setPosition (osgVector3(  m,  m,0.f));
        texts_[WindowManager::BOTTOM][WindowManager::CENTER]->setPosition (osgVector3(w/2,  m,0.f));
        texts_[WindowManager::BOTTOM][WindowManager::RIGHT ]->setPosition (osgVector3(w-m,  m,0.f));
        texts_[WindowManager::CENTER][WindowManager::LEFT  ]->setPosition (osgVector3(  m,h/2,0.f));
        texts_[WindowManager::CENTER][WindowManager::CENTER]->setPosition (osgVector3(w/2,h/2,0.f));
        texts_[WindowManager::CENTER][WindowManager::RIGHT ]->setPosition (osgVector3(w-m,h/2,0.f));
        texts_[WindowManager::TOP   ][WindowManager::LEFT  ]->setPosition (osgVector3(  m,h-m,0.f));
        texts_[WindowManager::TOP   ][WindowManager::CENTER]->setPosition (osgVector3(w/2,h-m,0.f));
        texts_[WindowManager::TOP   ][WindowManager::RIGHT ]->setPosition (osgVector3(w-m,h-m,0.f));
        return true;
      }

      osg::ref_ptr<osgText::Text>   texts_[3][3];
      osg::observer_ptr<osg::Camera> camera_;
    };

    struct WriteToFile : osgViewer::ScreenCaptureHandler::CaptureOperation
    {
      WriteToFile(const std::string& filename, const std::string& extension)
        : basename(filename), ext(extension), counter(0)
      {}


      virtual void operator()(const osg::Image& image, const unsigned int context_id)
      {
        std::stringstream filename;
        filename << basename << "_" << counter++ << '.' << ext;

        osgDB::writeImageFile(image, filename.str());

        OSG_INFO<<"ScreenCaptureHandler: Taking a screenshot, saved as '"<<filename.str()<<"'"<<std::endl;
      }

      const std::string basename;
      const std::string ext;
      unsigned int counter;
    };
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
    // Enable Outline highlight state.
    main_camera_->setCullMask(VisibilityBit);
    main_camera_->setClearMask(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    main_camera_->setClearColor(osg::Vec4(0.,0.,0.,0.));
    // Create background camera
    bg_camera_ = new osg::Camera;
    bg_camera_->setName("bg_camera");
    bg_camera_->setCullMask(VisibilityBit);
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
    bg_camera_->setCullingActive(false);
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

      bg_geom_  ->setDataVariance(osg::Object::STATIC);
      bg_camera_->setDataVariance(osg::Object::STATIC);
      geode     ->setDataVariance(osg::Object::STATIC);
      //bg_camera_->setGraphicsContext(camera->getGraphicsContext());
    }
    
    asGroup()->addChild(bg_camera_);
  }

  void WindowManager::createHUDcamera()
  {
    // Create HUD camera
    osg::ref_ptr <const osg::GraphicsContext::Traits> traits_ptr = gc_->getTraits();

    hud_camera_ = new osg::Camera;
    hud_camera_->setName("hud_camera");
    const osg::Node::NodeMask mask = ~IntersectionBit;
    hud_camera_->setNodeMask(mask);

    hud_camera_->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    hud_camera_->setClearMask( GL_DEPTH_BUFFER_BIT );
    hud_camera_->setRenderOrder( osg::Camera::POST_RENDER );
    hud_camera_->setAllowEventFocus( false );
    hud_camera_->setProjectionMatrix(
        osg::Matrix::ortho2D(traits_ptr->x,traits_ptr->width,traits_ptr->y,traits_ptr->height));
    hud_camera_->getOrCreateStateSet()->setMode(
        GL_LIGHTING, osg::StateAttribute::OFF );

    textGeode_ = new osg::Geode;
    hud_camera_->addChild (textGeode_);

    ResizeHandler* rh = new ResizeHandler(hud_camera_);
    static osg::ref_ptr<osgText::Font> font = defaultFont();
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j) {
        osg::ref_ptr<osgText::Text>& text      = texts_[i][j];
        text = new osgText::Text;
        text->setAxisAlignment( osgText::TextBase::XY_PLANE );
        text->setDataVariance( osg::Object::DYNAMIC );
        text->setFont( font );
        switch (i) {
          case TOP:
            switch (j) {
              case LEFT  : text->setAlignment( osgText::TextBase::  LEFT_TOP ); break;
              case CENTER: text->setAlignment( osgText::TextBase::CENTER_TOP ); break;
              case RIGHT : text->setAlignment( osgText::TextBase:: RIGHT_TOP ); break;
            }
            break;
          case CENTER:
            switch (j) {
              case LEFT  : text->setAlignment( osgText::TextBase::  LEFT_CENTER ); break;
              case CENTER: text->setAlignment( osgText::TextBase::CENTER_CENTER ); break;
              case RIGHT : text->setAlignment( osgText::TextBase:: RIGHT_CENTER ); break;
            }
            break;
          case BOTTOM:
            switch (j) {
              case LEFT  : text->setAlignment( osgText::TextBase::  LEFT_BOTTOM ); break;
              case CENTER: text->setAlignment( osgText::TextBase::CENTER_BOTTOM ); break;
              case RIGHT : text->setAlignment( osgText::TextBase:: RIGHT_BOTTOM ); break;
            }
            break;
        }

        rh->texts_[i][j] = text;
        textActive_[i][j] = false;
      }
    viewer_ptr_->addEventHandler (rh);

    // Property to scene
    addProperty (
        StringProperty::create("Text/TopLeft",
          boost::bind (&WindowManager::getText, this, TOP, LEFT),
          boost::bind (&WindowManager::setText, this, TOP, LEFT, _1, 20)));
    addProperty (
        StringProperty::create("Text/TopCenter",
          boost::bind (&WindowManager::getText, this, TOP, CENTER),
          boost::bind (&WindowManager::setText, this, TOP, CENTER, _1, 20)));
    addProperty (
        StringProperty::create("Text/TopRight",
          boost::bind (&WindowManager::getText, this, TOP, RIGHT),
          boost::bind (&WindowManager::setText, this, TOP, RIGHT, _1, 20)));
    addProperty (
        StringProperty::create("Text/CenterLeft",
          boost::bind (&WindowManager::getText, this, CENTER, LEFT),
          boost::bind (&WindowManager::setText, this, CENTER, LEFT, _1, 20)));
    addProperty (
        StringProperty::create("Text/CenterCenter",
          boost::bind (&WindowManager::getText, this, CENTER, CENTER),
          boost::bind (&WindowManager::setText, this, CENTER, CENTER, _1, 20)));
    addProperty (
        StringProperty::create("Text/CenterRight",
          boost::bind (&WindowManager::getText, this, CENTER, RIGHT),
          boost::bind (&WindowManager::setText, this, CENTER, RIGHT, _1, 20)));
    addProperty (
        StringProperty::create("Text/BottomLeft",
          boost::bind (&WindowManager::getText, this, BOTTOM, LEFT),
          boost::bind (&WindowManager::setText, this, BOTTOM, LEFT, _1, 20)));
    addProperty (
        StringProperty::create("Text/BottomCenter",
          boost::bind (&WindowManager::getText, this, BOTTOM, CENTER),
          boost::bind (&WindowManager::setText, this, BOTTOM, CENTER, _1, 20)));
    addProperty (
        StringProperty::create("Text/BottomRight",
          boost::bind (&WindowManager::getText, this, BOTTOM, RIGHT),
          boost::bind (&WindowManager::setText, this, BOTTOM, RIGHT, _1, 20)));
  }

  std::string WindowManager::getText (TextAlignment vPos, TextAlignment hPos) const
  {
    osg::ref_ptr<osgText::Text> text = texts_[vPos][hPos];
    if (!text) return std::string();
    else return text->getText().createUTF8EncodedString();
  }

  void WindowManager::setText (TextAlignment vPos, TextAlignment hPos,
      const std::string& content, float size)
  {
    if (content.size() == 0) {
      textGeode_->removeDrawable (texts_[vPos][hPos]);
      textActive_[vPos][hPos] = false;

      for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
          if (textActive_[i][j]) return;
      asGroup()->removeChild(hud_camera_);
      setDirty();
      return;
    }

    osg::ref_ptr<osgText::Text>& text      = texts_[vPos][hPos];
    if (!textActive_[vPos][hPos]) {
      textGeode_->addDrawable( text );
      textActive_[vPos][hPos] = true;
    }
    text->setCharacterSize( size );
    text->setText( content );

    if (!asGroup()->containsNode(hud_camera_)) {
      osg::Viewport* vp = viewer_ptr_->getCamera()->getViewport();
      viewer_ptr_->getEventQueue()->windowResize ((int)vp->x(), (int)vp->y(), (int)vp->width(), (int)vp->height());

      asGroup()->addChild(hud_camera_);
    }
    setDirty();
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
    osg::ref_ptr<ScreenShot> screenshot_ = new ScreenShot(filename);
    main_camera_->setFinalDrawCallback (screenshot_);
    viewer_ptr_->renderingTraversals();
    main_camera_->setFinalDrawCallback(0);
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
      ::osgViewer::ViewerRefPtr viewer = new ::osgViewer::Viewer();

      /* init main camera */
      ::osg::CameraRefPtr camera = viewer->getCamera ();
      camera->setName("main_camera");

      const osg::GraphicsContext::Traits* traits_ptr = gc->getTraits ();
      camera->setGraphicsContext(gc);
      camera->setViewport(new osg::Viewport(0,0, traits_ptr->width, traits_ptr->height));
      camera->setProjectionMatrixAsPerspective(
          30.0f, static_cast<double>(traits_ptr->width)/static_cast<double>(traits_ptr->height), 1.0f, 10000.0f );
      GLenum buffer = traits_ptr->doubleBuffer ? GL_BACK : GL_FRONT;
      camera->setDrawBuffer(buffer);
      camera->setReadBuffer(buffer);

      /* Disable small features culling */
      osg::CullStack::CullingMode cullingMode = camera->getCullingMode();
      cullingMode &= ~(osg::CullStack::SMALL_FEATURE_CULLING);
      camera->setCullingMode( cullingMode );

      /* avoid ending */
      viewer->setKeyEventSetsDone (0);
      viewer->addEventHandler(new osgViewer::HelpHandler);

      init (viewer, gc);
    }

    void WindowManager::init(osgViewer::Viewer* v, osg::GraphicsContext* gc)
    {
      setID (gc->getTraits()->windowName);
      
      viewer_ptr_ = v;
      viewer_ptr_->setSceneData ( asGroup() );
      lastSceneWasDisrty_ = true;
      
      /* init main camera */
      main_camera_ = viewer_ptr_->getCamera ();

      // Enable Outline highlight state.
      main_camera_->setClearStencil(0);
      osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
      if (ds->getMinimumNumStencilBits() == 0)
        ds->setMinimumNumStencilBits(1);
      
      gc_ = osg::GraphicsContextRefPtr (gc);
      createBackground();
      createManipulator();
      createHUDcamera();
    }

    WindowManager::WindowManager ()
      : GroupNode ("")
      , nodeTrackerManipulatorIndex(2)
    {
        init (0, 0, 600, 480);
    }

    WindowManager::WindowManager (osg::GraphicsContext* gc)
      : GroupNode ("")
      , nodeTrackerManipulatorIndex(2)
    {
        init (gc);
    }

    WindowManager::WindowManager (osgViewer::Viewer* v, osg::GraphicsContext* gc)
      : GroupNode ("")
      , nodeTrackerManipulatorIndex(2)
    {
        init (v, gc);
    }

    WindowManager::WindowManager (const unsigned int& x,
                                  const unsigned int& y,
                                  const unsigned int& width,
                                  const unsigned int& height)
      : GroupNode ("")
      , nodeTrackerManipulatorIndex(2)
    {
        init (x, y, width, height);
    }

    WindowManager::WindowManager (const WindowManager& other)
      : GroupNode (other)
      , nodeTrackerManipulatorIndex(2)
    {
      init (viewer_ptr_, gc_);
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
        bool result = addChild (graphical_object_ptr);
        return result;
    }

    bool WindowManager::done ()
    {
        return  viewer_ptr_->done();
    }

    bool WindowManager::frame ()
    {
      bool callFrame = screen_capture_ptr_;
      if (!callFrame) {
        IsDirtyVisitor isDirtyVisitor;
        accept (isDirtyVisitor);
        // FIXME For some reasons, when highlight state of a node is changed,
        // method frame must be called twice to get it rendered properly.
        // lastSceneWasDisrty_ forces to draw twice after a dirty scene.
        callFrame = lastSceneWasDisrty_
          || isDirtyVisitor.isDirty()
          || viewer_ptr_->checkNeedToDoFrame ();
        lastSceneWasDisrty_ = isDirtyVisitor.isDirty();
      }
      if (callFrame)
        viewer_ptr_->frame();
      else
        return false;

      SetCleanVisitor setCleanVisitor;
      accept (setCleanVisitor);
      return true;
    }

    bool WindowManager::run ()
    {
        return viewer_ptr_->run();
    }

    void WindowManager::setWindowDimension (const osgVector2& size)
    {
        /* Define new trait dimension of the main camera */
        const osg::GraphicsContext::Traits* traits_ptr = gc_->getTraits ();
        gc_->resized (traits_ptr->x, traits_ptr->y, (int)size[0], (int)size[1]);
    }

    osgVector2 WindowManager::getWindowDimension() const
    {
        osgVector2 dimention;
        const osg::GraphicsContext::Traits* traits_ptr = gc_->getTraits ();
        dimention.x() = (osg::Vec2f::value_type) traits_ptr->width;
        dimention.y() = (osg::Vec2f::value_type) traits_ptr->height;
        return dimention;
    }

    void WindowManager::setWindowPosition (const osgVector2& position)
    {
        /* Define new trait dimension of the main camera */
        const osg::GraphicsContext::Traits* traits_ptr = gc_->getTraits ();
        gc_->resized ((int)position[0], (int)position[1],
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
    typedef osgViewer::ScreenCaptureHandler SCH_t;
    osg::ref_ptr<WriteToFile> wtf = new WriteToFile (filename, extension);
    screen_capture_ptr_ = new SCH_t (wtf.get(), -1);
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
    // Remove event handler
    typedef osgViewer::View::EventHandlers EventHandlers;
    EventHandlers& handlers = viewer_ptr_->getEventHandlers();
    EventHandlers::iterator _handler =
      std::find (handlers.begin(), handlers.end(), screen_capture_ptr_);
    if (_handler != handlers.end()) {
      handlers.erase (_handler);
      screen_capture_ptr_.release();
    }
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
    rot = matrix.getRotate();
    pos = matrix.getTrans();
  }

  void WindowManager::setCameraTransform(const osg::Vec3d &pos, const osg::Quat &rot){
   osg::Matrixd matrix= osg::Matrixd();
   matrix.setTrans(pos.x(),pos.y(),pos.z());
   matrix.setRotate(rot);
   manipulator_ptr->setByMatrix(matrix);
   lastSceneWasDisrty_=true;
  }

  
    /* End declaration of public function members */

} /* namespace viewer */

} /* namespace gepetto */
