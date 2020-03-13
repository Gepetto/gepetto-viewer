// Copyright (c) 2015-2018, LAAS-CNRS
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of gepetto-viewer.
// gepetto-viewer is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// gepetto-viewer is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer. If not, see <http://www.gnu.org/licenses/>.

#include "gepetto/gui/osgwidget.hh"
#include "gepetto/gui/mainwindow.hh"
#include <gepetto/gui/pick-handler.hh>

#include <QAction>
#include <QFileDialog>
#include <QProcess>
#include <QTextBrowser>
#include <QDockWidget>

#include <osg/Camera>

#include <osg/DisplaySettings>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/StateSet>

#include <osgGA/EventQueue>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>

#include <cassert>

#include <stdexcept>
#include <vector>

#include <QDebug>
#include <QKeyEvent>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
# include <QStandardPaths>
#endif
#include <QToolBar>
#include <QVBoxLayout>
#include <QWheelEvent>

#include <gepetto/viewer/urdf-parser.h>
#include <gepetto/viewer/OSGManipulator/keyboard-manipulator.h>
#include <gepetto/viewer/macros.h>
#include <gepetto/viewer/window-manager.h>

#include <gepetto/gui/windows-manager.hh>
#include <gepetto/gui/selection-event.hh>
#include <gepetto/gui/action-search-bar.hh>

namespace gepetto {
  namespace gui {
    OSGWidget::OSGWidget(WindowsManagerPtr_t wm,
                         const std::string & name,
                         MainWindow *parent, Qt::WindowFlags f ,
                         osgViewer::ViewerBase::ThreadingModel threadingModel)
    : QWidget( parent, f )
    , graphicsWindow_()
    , wsm_ (wm)
    , pickHandler_ (new PickHandler (this, wsm_))
    , wid_ ()
    , wm_ ()
    , viewer_ (new osgViewer::Viewer)
    , screenCapture_ ()
    , toolBar_ (new QToolBar (QString::fromStdString(name) + " tool bar"))
    , process_ (new QProcess (this))
    , showPOutput_ (new QDialog (this, Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint))
    , pOutput_ (new QTextBrowser())
    , fullscreen_ (new QWidget (NULL, Qt::Window | Qt::WindowStaysOnTopHint))
    {
      initGraphicsWindowsAndViewer (parent, name);
      initToolBar ();

      wid_ = wm->createWindow (name, this, viewer_, graphicsWindow_.get());
      wm_ = wsm_->getWindowManager (wid_);

      viewer_->setThreadingModel(threadingModel);

      osgQt::GLWidget* glWidget = graphicsWindow_->getGLWidget();
      //glWidget->setForwardKeyEvents(true);
      QVBoxLayout* hblayout = new QVBoxLayout (this);
      hblayout->setContentsMargins(1,1,1,1);
      setLayout (hblayout);
      hblayout->addWidget(toolBar_);
      hblayout->addWidget(glWidget);
      glWidget->setMinimumSize(50,10);

      // TODO Adding the properties here is problematic. They won't be
      // shown in the GUI because the display is created before this code is run.
      wm_->addProperty (
        viewer::BoolProperty::create("WindowFixedSize",
          viewer::BoolProperty::getterFromMemberFunction (this, &OSGWidget::isFixedSize),
          viewer::BoolProperty::setterFromMemberFunction (this, &OSGWidget::setFixedSize)));
      wm_->addProperty (
        viewer::Vector2Property::create("WindowSize",
          viewer::Vector2Property::getterFromMemberFunction (wm_.get(), &viewer::WindowManager::getWindowDimension),
          viewer::Vector2Property::Setter_t()));

      connect( &timer_, SIGNAL(timeout()), this, SLOT(update()) );
      timer_.start(parent->settings_->refreshRate);

      // Setup widgets to record movies.
      process_->setProcessChannelMode(QProcess::MergedChannels);
      connect (process_, SIGNAL (readyReadStandardOutput ()), SLOT (readyReadProcessOutput()));
      showPOutput_->setModal(false);
      showPOutput_->setLayout(new QHBoxLayout ());
      showPOutput_->layout()->addWidget(pOutput_);
    }

    OSGWidget::~OSGWidget()
    {
      viewer_->setDone(true);
      viewer_->removeEventHandler(pickHandler_);
      pickHandler_ = NULL;
      wm_.reset();
      wsm_.reset();
      delete fullscreen_;
    }

    void OSGWidget::paintEvent(QPaintEvent*)
    {
      viewer::ScopedLock lock(wsm_->osgFrameMutex());
      wm_->frame();
    }

    WindowsManager::WindowID OSGWidget::windowID() const
    {
      return wid_;
    }

    WindowManagerPtr_t OSGWidget::window() const
    {
      return wm_;
    }

    WindowsManagerPtr_t OSGWidget::osg() const
    {
      return wsm_;
    }

    void OSGWidget::onHome()
    {
      viewer_->home ();
    }

    void OSGWidget::addFloor()
    {
      wsm_->addFloor("hpp-gui/floor");
    }

    void OSGWidget::toggleCapture (bool active)
    {
      MainWindow* main = MainWindow::instance();
      if (active) {
        QDir tmp ("/tmp");
        tmp.mkpath ("gepetto-gui/record"); tmp.cd("gepetto-gui/record");
        foreach (QString f, tmp.entryList(QStringList() << "img_0_*.jpeg", QDir::Files))
          tmp.remove(f);
        QString path = tmp.absoluteFilePath("img");
        const char* ext = "jpeg";
        osg ()->startCapture(windowID(), path.toLocal8Bit().data(), ext);
        main->log("Saving images to " + path + "_*." + ext);
      } else {
        osg()->stopCapture(windowID());
        QString outputFile = QFileDialog::getSaveFileName(this, tr("Save video to"), "untitled.mp4");
        if (!outputFile.isNull()) {
          if (QFile::exists(outputFile))
            QFile::remove(outputFile);
          QString avconv = main->settings_->avconv;

          QStringList args;
          QString input = "/tmp/gepetto-gui/record/img_0_%d.jpeg";
          args << main->settings_->avConvInputOptions
            << "-i" << input
            << main->settings_->avConvOutputOptions
            << outputFile;
          qDebug () << args;

          showPOutput_->setWindowTitle(avconv + " " + args.join(" "));
          pOutput_->clear();
          showPOutput_->resize(main->size() / 2);
          showPOutput_->show();
          process_->start(avconv, args);
          bool started = process_->waitForStarted(-1);
          if (!started) {
            showPOutput_->hide();
            switch (process_->error()) {
              case QProcess::FailedToStart:
                main->logError ("Failed to start " + avconv + ". Either it is missing, "
                    "or you may have insufficient permissions to invoke it.\n");
                break;
              case QProcess::Crashed      :
                main->logError ("" + avconv + " crashed some time after starting successfully.");
                break;
              case QProcess::Timedout     :
              case QProcess::WriteError   :
              case QProcess::ReadError    :
              case QProcess::UnknownError :
                main->logError ("An unknown error made " + avconv + " stopped before "
                    "finishing.");
                break;
            }
            main->logError ("You can manually run\n" + avconv + " " + args.join(" "));
          }
        }
      }
    }

    void OSGWidget::captureFrame ()
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
      QString pathname = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
#else
      QString pathname = QDir::home().filePath("Pictures");
#endif
      if (pathname.isEmpty()) {
        qDebug() << "Unable to get the directory where to write images";
        return;
      }
      QDir path (pathname);
      if (!path.mkpath(".")) {
        qDebug() << "Unable to create directory" << pathname;
        return;
      }
      QString filename (path.filePath (
            QDateTime::currentDateTime().toString("yyyy-MM-dd.hh-mm-ss'.png'")));
      captureFrame (filename.toStdString());
    }

    void OSGWidget::captureFrame (const std::string& filename)
    {
      viewer::ScopedLock lock(wsm_->osgFrameMutex());
      wm_->captureFrame (filename);
    }

    bool OSGWidget::startCapture (const std::string& filename,
        const std::string& extension)
    {
      viewer::ScopedLock lock(wsm_->osgFrameMutex());
      wm_->startCapture (filename, extension);
      return true;
    }

    bool OSGWidget::stopCapture ()
    {
      viewer::ScopedLock lock(wsm_->osgFrameMutex());
      wm_->stopCapture ();
      return true;
    }

    bool OSGWidget::isFixedSize () const
    {
      try {
        return wm_->property("WindowSize")->hasWriteAccess();
      } catch (const std::invalid_argument&) {
        return false;
      }
    }

    void OSGWidget::setFixedSize (bool fixedSize)
    {
      try {
        viewer::Property& size(*wm_->property("WindowSize"));
        if (size.hasWriteAccess() == fixedSize) return;

        // Cast to Vector2Property
        viewer::Vector2Property& vsize (dynamic_cast<viewer::Vector2Property&>(size));
        osgQt::GLWidget* glWidget = graphicsWindow_->getGLWidget();
        if (fixedSize) {
          glWidget->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
          // Add write access
          vsize.setter (viewer::Vector2Property::Setter_t(
                viewer::Vector2Property::setterFromMemberFunction (this, &OSGWidget::setWindowDimension)
                ));
        } else {
          glWidget->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Preferred);
          // Remove write access
          vsize.setter (viewer::Vector2Property::Setter_t());
        }
      } catch (const std::invalid_argument&) {
        return;
      } catch (const std::bad_cast&) {
        return;
      }
    }

    void OSGWidget::setWindowDimension (const osgVector2& size)
    {
      osgQt::GLWidget* glWidget = graphicsWindow_->getGLWidget();
      if (isFixedSize()) {
        glWidget->resize ((int)size[0], (int)size[1]);
        glWidget->setMinimumSize((int)size[0], (int)size[1]);
      } else
        glWidget->setMinimumSize(50,10);
    }

    void OSGWidget::readyReadProcessOutput()
    {
      pOutput_->append(process_->readAll());
    }

    QIcon iconFromTheme (const QString& name)
    {
      QIcon icon;
      if (QIcon::hasThemeIcon(name)) {
        icon = QIcon::fromTheme(name);
      } else {
        icon.addFile(QString::fromUtf8(""), QSize(), QIcon::Normal, QIcon::Off);
      }
      return icon;
    }

    void OSGWidget::initToolBar ()
    {
      toolBar_->addAction(
          iconFromTheme("zoom-fit-best"), "Zoom to fit", this, SLOT(onHome()))
        ->setToolTip("Zoom to fit");

      QIcon icon;
      icon.addFile(QString::fromUtf8(":/icons/floor.png"), QSize(), QIcon::Normal, QIcon::Off);
      QAction* addFloor = toolBar_->addAction(icon, "Add floor", this, SLOT(addFloor()));
      addFloor->setToolTip("Add a floor");
      MainWindow* main = MainWindow::instance();
      main->actionSearchBar()->addAction (addFloor);

      toolBar_->addAction(iconFromTheme("insert-image"), "Take snapshot",
          this, SLOT(captureFrame()))
        ->setToolTip("Take a snapshot");

      QAction* recordMovie = toolBar_->addAction(iconFromTheme("media-record"), "Record movie");
      connect(recordMovie, SIGNAL(triggered(bool)), SLOT(toggleCapture(bool)), Qt::QueuedConnection);
      recordMovie->setCheckable (true);
      recordMovie->setToolTip("Record the central widget as a sequence of images. You can find the images in /tmp/gepetto-gui/record/img_%d.jpeg");

      QAction* toggleFullscreen = new QAction(iconFromTheme("view-fullscreen"),
          "Toggle fullscreen mode", this);
      toggleFullscreen->setShortcut (Qt::SHIFT | Qt::Key_F);
      toggleFullscreen->setCheckable (true);
      toggleFullscreen->setChecked (false);
      connect(toggleFullscreen, SIGNAL(toggled(bool)), SLOT(toggleFullscreenMode(bool)));
      toolBar_->addAction (toggleFullscreen);

      fullscreen_->setLayout(new QVBoxLayout);
      fullscreen_->layout()->setContentsMargins (0,0,0,0);
    }

    void OSGWidget::initGraphicsWindowsAndViewer (MainWindow* parent, const std::string& name)
    {
      osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
      osg::ref_ptr <osg::GraphicsContext::Traits> traits_ptr (new osg::GraphicsContext::Traits(ds));
      traits_ptr->windowName = name;
      traits_ptr->x = this->x();
      traits_ptr->y = this->y();
      traits_ptr->width = this->width();
      traits_ptr->height = this->height();
      traits_ptr->windowDecoration = false;
      traits_ptr->doubleBuffer = true;
      traits_ptr->vsync = true;
      //  traits_ptr->sharedContext = 0;

      graphicsWindow_ = new osgQt::GraphicsWindowQt ( traits_ptr );

      osg::Camera* camera = viewer_->getCamera();
      camera->setGraphicsContext( graphicsWindow_ );

      camera->setClearColor( osg::Vec4(0.2f, 0.2f, 0.6f, 1.0f) );
      camera->setViewport( new osg::Viewport(0, 0, traits_ptr->width, traits_ptr->height) );
      camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits_ptr->width)/static_cast<double>(traits_ptr->height), 1.0f, 10000.0f );

      viewer_->setKeyEventSetsDone(0);

      // Manipulators
      osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
      keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
      keyswitchManipulator->addMatrixManipulator( '2', "First person", new ::osgGA::KeyboardManipulator(graphicsWindow_));
      keyswitchManipulator->selectMatrixManipulator (0);
      viewer_->setCameraManipulator( keyswitchManipulator.get() );

      // Event handlers
      screenCapture_ = new osgViewer::ScreenCaptureHandler (
            new osgViewer::ScreenCaptureHandler::WriteToFile (
              parent->settings_->captureDirectory + "/" + parent->settings_->captureFilename,
              parent->settings_->captureExtension),
            1);
      viewer_->addEventHandler(screenCapture_);
      viewer_->addEventHandler(new osgViewer::HelpHandler);
      viewer_->addEventHandler(pickHandler_);
      viewer_->addEventHandler(new osgViewer::StatsHandler);
    }

    void OSGWidget::toggleFullscreenMode (bool fullscreenOn)
    {
      if (!isVisible()) return;
      if (fullscreenOn) {
        QDockWidget* dockOSG = qobject_cast<QDockWidget*>(this->parentWidget());
        if (dockOSG) {
          normal_ = this->parentWidget();
          fullscreen_->layout()->addWidget (this);
          fullscreen_->showFullScreen();
        }
      } else {
        QDockWidget* dockOSG = qobject_cast<QDockWidget*>(normal_);
        if (dockOSG) {
          fullscreen_->hide();
          dockOSG->setWidget (this);
        }
      }
    }
  } // namespace gui
} // namespace gepetto
