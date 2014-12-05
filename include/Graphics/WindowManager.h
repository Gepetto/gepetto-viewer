//
//  WindowManager.h
//  ScneViewer
//
//  Created by Justin Carpentier, Mathieu Geisert in  November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef SCENEVIEWER_WINDOWMANAGER_HH
#define SCENEVIEWER_WINDOWMANAGER_HH

#include <osgViewer/Viewer>
#include "GroupNode.h"
#include "Node.h"

namespace Graphics {

    DEF_CLASS_SMART_PTR(WindowManager)

    class WindowManager
    {
    private:

        /** GraphicalEngine    */
        unsigned int width_window_dimension_, height_window_dimension_;
        unsigned int x_window_position_, y_window_position_;

        bool window_decoration_status_;

        /** Scene Graphical Group */
        GroupNodePtr_t scene_ptr_;

        /** OSG viewer */
        ::osgViewer::ViewerRefPtr viewer_ptr_;

        /** OSG cameras */
        ::osg::CameraRefPtr main_camera_;
        ::osg::TraitsRefPtr traits_ptr_;

        ::osg::CameraRefPtr hud_camera_;


        /** Associated weak pointer */
        WindowManagerWeakPtr weak_ptr_;

        void init(const unsigned int& x, const unsigned int& y, const unsigned int& width, const unsigned int& height);

        WindowManager();

        WindowManager(const unsigned int& x, const unsigned int& y, const unsigned int& width, const unsigned int& height);

        WindowManager(const WindowManager& other);

        /** Initialize weak_ptr */
        void initWeakPtr (WindowManagerWeakPtr other_weak_ptr);
    protected:



    public:

        /** Create and initialize a graphical engine of type OSG
         */
        static WindowManagerPtr_t create();

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
        virtual GroupNodePtr_t getScene() const { return scene_ptr_->self(); }

        /** Return the current doing of procedure
         */
        virtual bool done ();

        /** Generate a new frame rendering
         */
        virtual bool frame ();

        /** Run the scene process
         */
        virtual bool run ();

        /** Define dimension of the window */
        virtual void setWindowDimension (const unsigned int& width, const unsigned int& height);

        /** Define the window position */
        virtual void setWindowPosition (const unsigned int& x_position, const unsigned int& y_position);

        /** Define the window decoration state */
        virtual void setWindowDecoration (bool window_decoration_status);

        /** Return the window x and y position as a 2D vector */
        osgVector2 getWindowPosition () const;

        /** Return the current window decoration status */
        bool getWindowDecoration () const { return window_decoration_status_; }

        /** Return the window width and height as a 2D vector */
        osgVector2 getWindowDimension () const;

        virtual ~WindowManager();
    };
} /* namespace Graphics */

#endif /* SCENEVIEWER_WINDOWMANAGER_HH */
