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

#ifndef GEPETTO_GUI_OSGWIDGET_HH
#define GEPETTO_GUI_OSGWIDGET_HH

#include <gepetto/gui/fwd.hh>
// This include must be include before any other Qt include for GLDEBUGPROC
#include <gepetto/viewer/config-osg.h>

#include <QLabel>
#include <QString>
#include <QThread>
#include <QTimer>
#include <osg/ref_ptr>
#include <osgQt/GraphicsWindowQt>
#include <osgViewer/ViewerBase>
#include <osgViewer/ViewerEventHandlers>

class QToolBar;
class QProcess;
class QTextBrowser;
class QTemporaryDir;

namespace gepetto {
namespace gui {
typedef viewer::WindowManagerPtr_t WindowManagerPtr_t;
typedef viewer::WindowID WindowID;

/// Widget that displays scenes.
class OSGWidget : public QWidget {
  Q_OBJECT
 public:
  OSGWidget(WindowsManagerPtr_t wm, const std::string& name, MainWindow* parent,
            Qt::WindowFlags f = 0,
            osgViewer::ViewerBase::ThreadingModel threadingModel =
                osgViewer::Viewer::ThreadPerContext);

  virtual ~OSGWidget();

  WindowID windowID() const;

  WindowManagerPtr_t window() const;

  WindowsManagerPtr_t osg() const;

 public slots:
  /// Replace the camera at her home position.
  void onHome();

  void addFloor();

  void toggleCapture(bool active);

  void captureFrame();
  void captureFrame(const std::string& filename);
  bool startCapture(const std::string& filename, const std::string& extension);
  bool stopCapture();

 protected:
  virtual void paintEvent(QPaintEvent* event);

  bool isFixedSize() const;

  void setFixedSize(bool fixedSize);

  void setWindowDimension(const osgVector2& size);

 private slots:
  void readyReadProcessOutput();
  void toggleFullscreenMode(bool fullscreenOn);

 private:
  void initToolBar();
  void initGraphicsWindowsAndViewer(MainWindow* parent,
                                    const std::string& name);

  osg::ref_ptr<osgQt::GraphicsWindowQt> graphicsWindow_;
  WindowsManagerPtr_t wsm_;
  osg::ref_ptr<PickHandler> pickHandler_;
  WindowID wid_;
  WindowManagerPtr_t wm_;
  QTimer timer_;
  int nSuccessiveStaticFrames_;
  osgViewer::ViewerRefPtr viewer_;
  osg::ref_ptr<osgViewer::ScreenCaptureHandler> screenCapture_;
  QTemporaryDir* tmpDirectory_;

  QToolBar* toolBar_;
  QAction* recordMovie_;

  // To record movies.
  QProcess* process_;
  QDialog* showPOutput_;
  QTextBrowser* pOutput_;

  QWidget *fullscreen_, *normal_;

  friend class PickHandler;
};
}  // namespace gui
}  // namespace gepetto

#endif  // GEPETTO_GUI_OSGWIDGET_HH
