//
//  window-manager.h
//  ScneViewer
//
//  Created by Justin Carpentier, Mathieu Geisert in  November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_WINDOWMANAGER_HH
#define GEPETTO_VIEWER_WINDOWMANAGER_HH

#include <osgGA/KeySwitchMatrixManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <gepetto/viewer/group-node.h>
#include <gepetto/viewer/node.h>

namespace gepetto {
namespace viewer {

    DEF_CLASS_SMART_PTR(WindowManager)

    /// Manage a window that renders a scene.
    /// The root of the rendered scene is a \ref GroupNode.
    class WindowManager : public GroupNode
    {
    private:
        const int nodeTrackerManipulatorIndex;

        /** Scene Graphical Group */
        GroupNodePtr_t scene_ptr_;

        /** OSG viewer */
        ::osgViewer::ViewerRefPtr viewer_ptr_;

        /** OSG cameras */
        ::osg::CameraRefPtr main_camera_;
        ::osg::GraphicsContextRefPtr gc_;
      
      /** Backgound camera */
      ::osg::CameraRefPtr bg_camera_;
      ::osg::Vec4 bg_color1_;
      ::osg::Vec4 bg_color2_;
      ::osg::GeometryRefPtr bg_geom_;

        /* OSG Screen capture handler */
        osg::ref_ptr < ::osgViewer::ScreenCaptureHandler> screen_capture_ptr_;

        /** Heads-Up Display (HUD) camera */
        ::osg::CameraRefPtr hud_camera_;
        osg::ref_ptr<osgText::Text> texts_[3][3];
        osg::ref_ptr<osg::Geode> textGeode_;
        bool textActive_[3][3];

        bool lastSceneWasDisrty_;

	osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> manipulator_ptr;
        /** Associated weak pointer */
        WindowManagerWeakPtr weak_ptr_;

      void createManipulator();
      
      void createBackground();
      void applyBackgroundColor();

        void createHUDcamera();

        void init(osg::GraphicsContext* gc);

        void init(osgViewer::Viewer* v, osg::GraphicsContext* gc);

        void init(const unsigned int& x, const unsigned int& y, const unsigned int& width, const unsigned int& height);

        WindowManager();

        WindowManager (osgViewer::Viewer* v, osg::GraphicsContext* gc);

        WindowManager (osg::GraphicsContext* gc);

        WindowManager(const unsigned int& x, const unsigned int& y, const unsigned int& width, const unsigned int& height);

        WindowManager(const WindowManager& other);

        /** Initialize weak_ptr */
        void initWeakPtr (WindowManagerWeakPtr other_weak_ptr);
    protected:

    public:
        enum TextAlignment {
          TOP    = 0,
          CENTER = 1,
          BOTTOM = 2,
          RIGHT  = 0,
          LEFT   = 2
        };

        /** Create and initialize a graphical engine of type OSG
         */
        static WindowManagerPtr_t create();

        /** Create and initialize a graphical engine with a GraphicsContext
         */
        static WindowManagerPtr_t create(osg::GraphicsContext* gc);

        static WindowManagerPtr_t create(osgViewer::Viewer* v, osg::GraphicsContext* gc);

        /** Create and initialize a graphical engine of type OSG with some parameters : position + dimension
         */
        static WindowManagerPtr_t create(const unsigned int& x, const unsigned int& y, const unsigned int& width, const unsigned int& height);

        /** Static method for creating a clone of box other with the copy constructor
         */
        static WindowManagerPtr_t createCopy(WindowManagerPtr_t other);

        /** Proceed to a clonage of the current object defined by the copy constructor
         */
        virtual WindowManagerPtr_t clone(void) const;

        /** Return a shared pointer of the current object
         */
        WindowManagerPtr_t self(void) const;

        /** Add a graphical object to the scene
         */
        virtual bool addNode(NodePtr_t graphical_object_ptr);

        /** Return the scene group
         */
        virtual GroupNodePtr_t getScene() const { return self(); }

        /** Return the current doing of procedure
         */
        virtual bool done ();

        /** Generate a new frame rendering
         */
        virtual bool frame ();

        /** Run the scene process
         */
        virtual bool run ();

        /** Define dimension of the window
         *  \param size (width, height)
         **/
        virtual void setWindowDimension (const osgVector2& size);

        /** Define the window position
         *  \param position (x_pos, y_pos)
         **/
        virtual void setWindowPosition (const osgVector2& position);

        /** Return the window x and y position as a 2D vector */
        osgVector2 getWindowPosition () const;

        /** Return the window width and height as a 2D vector */
        osgVector2 getWindowDimension () const;

        std::string getText (TextAlignment verticalPos, TextAlignment horizontalPos) const;

        /** Set the HUD text */
        void setText (TextAlignment verticalPos, TextAlignment horizontalPos,
            const std::string& text, float size = 20);

        /** Return a ref to the viewer */
        ::osgViewer::ViewerRefPtr getViewerClone();

        virtual ~WindowManager();

        void captureFrame (const std::string& filename);

        void startCapture (const std::string& filename,
            const std::string& extension);

        void stopCapture ();

        bool writeNodeFile (const std::string& filename);
        
      void setBackgroundColor1(const osg::Vec4 & color)
      { bg_color1_ = color; applyBackgroundColor(); }
        
      void setBackgroundColor2(const osg::Vec4 & color)
      { bg_color2_ = color; applyBackgroundColor(); }

      void getCameraTransform(osg::Vec3d &pos, osg::Quat &rot);
      void setCameraTransform(const osg::Vec3d &pos, const osg::Quat &rot);

	void attachCameraToNode(NodePtr_t node);
	void detachCamera();

    };
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_WINDOWMANAGER_HH */
