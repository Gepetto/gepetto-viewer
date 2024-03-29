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

#include "gepetto/gui/pick-handler.hh"

#include <gepetto/viewer/node.h>

#include <QApplication>
#include <QDebug>
#include <QStatusBar>
#include <gepetto/gui/bodytreewidget.hh>
#include <gepetto/gui/mainwindow.hh>
#include <gepetto/gui/osgwidget.hh>
#include <gepetto/gui/selection-event.hh>
#include <gepetto/gui/selection-handler.hh>
#include <gepetto/gui/tree-item.hh>
#include <gepetto/gui/windows-manager.hh>
#include <iostream>
#include <osg/io_utils>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osgViewer/Viewer>

#include "point-intersector.hh"

namespace gepetto {
namespace gui {
PickHandler::PickHandler(OSGWidget* parent, WindowsManagerPtr_t wsm)
    : osgGA::GUIEventHandler(),
      wsm_(wsm),
      parent_(parent),
      pushed_(false),
      lastX_(0),
      lastY_(0),
      lineIntersector_(new osgUtil::LineSegmentIntersector(
          osgUtil::Intersector::WINDOW, 0., 0.)),
      pointIntersector_(
          new PointIntersector(osgUtil::Intersector::WINDOW, 0., 0.)) {
  lineIntersector_->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);
  pointIntersector_->setPickBias(0.01f);
  pointIntersector_->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);
}

PickHandler::~PickHandler() {}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea,
                         osgGA::GUIActionAdapter& aa) {
  switch (ea.getEventType()) {
    case osgGA::GUIEventAdapter::PUSH:
    case osgGA::GUIEventAdapter::RELEASE:
      if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
        if (pushed_ && ea.getEventType() == osgGA::GUIEventAdapter::RELEASE) {
          pushed_ = false;
          if ((int)floor(lastX_ - ea.getX() + 0.5) == 0 &&
              (int)floor(lastY_ - ea.getY() + 0.5) == 0) {
            selectionNodeUnderCursor(aa, ea.getX(), ea.getY(),
                                     ea.getModKeyMask());
            return true;
          }
        }
        if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH) {
          lastX_ = ea.getX();
          lastY_ = ea.getY();
          pushed_ = true;
        }
      }
      return false;
      break;
    case osgGA::GUIEventAdapter::KEYUP:
      switch (ea.getKey()) {
        case 'z':
          setCameraToSelected(aa, false);
          return true;
        case 'Z':
          setCameraToSelected(aa, true);
          return true;
        case 'f':
          centerViewToMouse(aa, ea.getX(), ea.getY());
          return true;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return false;
}

void PickHandler::getUsage(osg::ApplicationUsage& usage) const {
  usage.addKeyboardMouseBinding("Right click", "Select node");
  usage.addKeyboardMouseBinding('z', "Move camera on selected node");
  usage.addKeyboardMouseBinding('Z', "Move and zoom on selected node");
  usage.addKeyboardMouseBinding('f', "Center view to mouse");
}

void PickHandler::computeLineIntersection(osgGA::GUIActionAdapter& aa,
                                          const float& x, const float& y) {
  lineIntersector_->reset();
  lineIntersector_->setStart(osg::Vec3d(x, y, 0.));
  lineIntersector_->setEnd(osg::Vec3d(x, y, 1.));

  osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
  if (viewer) {
    // There is no need to lock the windows manager mutex
    // as this is treated in the event loop of OSG, and not Qt.
    // On the contrary, locking here creates a deadlock as the lock is
    // already acquired by OSGWidget::paintEvent.
    // wsm_->lock().lock();
    // osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
    // new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, x, y);
    // intersector->setIntersectionLimit( osgUtil::Intersector::LIMIT_NEAREST );

    osgUtil::IntersectionVisitor iv(lineIntersector_);
    iv.setTraversalMask(viewer::IntersectionBit);

    osg::Camera* camera = viewer->getCamera();
    camera->accept(iv);
  }
}

PickHandler::LineSegmentIntersector PickHandler::computeLineOrPointIntersection(
    osgGA::GUIActionAdapter& aa, const float& x, const float& y) {
  osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
  if (!viewer) return LineSegmentIntersector();
  osg::Camera* camera = viewer->getCamera();

  lineIntersector_->reset();
  lineIntersector_->setStart(osg::Vec3d(x, y, 0.));
  lineIntersector_->setEnd(osg::Vec3d(x, y, 1.));
  {
    osgUtil::IntersectionVisitor iv(lineIntersector_);
    iv.setTraversalMask(viewer::IntersectionBit);
    camera->accept(iv);
  }
  if (lineIntersector_->containsIntersections()) return lineIntersector_;

  pointIntersector_->reset();
  pointIntersector_->setStart(osg::Vec3d(x, y, 0.));
  pointIntersector_->setEnd(osg::Vec3d(x, y, 1.));
  {
    osgUtil::IntersectionVisitor iv(pointIntersector_);
    iv.setTraversalMask(viewer::IntersectionBit);
    // iv.setTraversalMask(viewer::IntersectionBit | viewer::EditionBit);
    camera->accept(iv);
  }
  if (pointIntersector_->containsIntersections()) return pointIntersector_;
  return LineSegmentIntersector();
}

void PickHandler::selectionNodeUnderCursor(osgGA::GUIActionAdapter& aa,
                                           const float& x, const float& y,
                                           int modKeyMask) {
  LineSegmentIntersector li = computeLineOrPointIntersection(aa, x, y);
  BodyTreeWidget* bt = MainWindow::instance()->bodyTree();

  if (!li || !li->containsIntersections()) {
    bt->emitBodySelected(new SelectionEvent(SelectionEvent::FromOsgWindow,
                                            QApplication::keyboardModifiers()));
    return;
  }

  // Only one intersection. Otherwise, one has to loop on elements of
  // intersector->getIntersections();
  const osgUtil::LineSegmentIntersector::Intersection& intersection =
      li->getFirstIntersection();
  for (int i = (int)intersection.nodePath.size() - 1; i >= 0; --i) {
    if (!(intersection.nodePath[i]->getNodeMask() & viewer::NodeBit)) continue;
    NodePtr_t n = wsm_->getNode(intersection.nodePath[i]->getName());
    if (n) {
      SelectionEvent* event = new SelectionEvent(
          SelectionEvent::FromOsgWindow, n, mapper_.getQtModKey(modKeyMask));
      event->setupIntersection(intersection);
      bt->emitBodySelected(event);

      QStatusBar* statusBar = MainWindow::instance()->statusBar();
      statusBar->clearMessage();
      if (osg::dynamic_pointer_cast<PointIntersector>(li))
        statusBar->showMessage(QString::fromStdString(n->getID()) +
                               QString(" - Vectex index: ") +
                               QString::number(intersection.primitiveIndex));
      else
        statusBar->showMessage(QString::fromStdString(n->getID()));
      return;
    }
  }
}

void PickHandler::centerViewToMouse(osgGA::GUIActionAdapter& aa, const float& x,
                                    const float& y) {
  osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
  if (!viewer) return;

  computeLineIntersection(aa, x, y);
  if (!lineIntersector_->containsIntersections()) return;

  // Only one intersection. Otherwise, one has to loop on elements of
  // intersector->getIntersections();
  const osgUtil::LineSegmentIntersector::Intersection& intersection =
      lineIntersector_->getFirstIntersection();

  osg::Vec3f P(intersection.getWorldIntersectPoint());

  osgGA::TrackballManipulator* tbm = dynamic_cast<osgGA::TrackballManipulator*>(
      viewer->getCameraManipulator());
  if (!tbm) {
    osgGA::KeySwitchMatrixManipulator* ksm =
        dynamic_cast<osgGA::KeySwitchMatrixManipulator*>(
            viewer->getCameraManipulator());
    if (ksm) {
      tbm = dynamic_cast<osgGA::TrackballManipulator*>(
          ksm->getCurrentMatrixManipulator());
    }
  }
  if (tbm) {
    tbm->setCenter(P);
  } else {
    osg::Vec3f eye, center, up;
    viewer->getCameraManipulator()->getInverseMatrix().getLookAt(eye, center,
                                                                 up);

    osg::Vec3f u(center - eye);
    u.normalize();
    osg::Vec3f v(up ^ u);
    osg::Vec3f t(v * (v * (P - eye)));
    eye += t;
    center += t;
    viewer->getCameraManipulator()->setByInverseMatrix(
        osg::Matrix::lookAt(eye, center, up));
  }
}

void PickHandler::setCameraToSelected(osgGA::GUIActionAdapter& aa, bool zoom) {
  MainWindow* main = MainWindow::instance();
  NodePtr_t last = main->osg()->getNode(
      main->selectionHandler()->mode()->currentBody().toStdString());
  if (!last) return;
  osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
  if (!viewer) return;

  const osg::BoundingSphere& bs = last->asGroup()->getBound();
  osg::Vec3f eye, center, up;
  viewer->getCameraManipulator()->getInverseMatrix().getLookAt(eye, center, up);

  osgGA::TrackballManipulator* tbm = dynamic_cast<osgGA::TrackballManipulator*>(
      viewer->getCameraManipulator());
  if (!tbm) {
    osgGA::KeySwitchMatrixManipulator* ksm =
        dynamic_cast<osgGA::KeySwitchMatrixManipulator*>(
            viewer->getCameraManipulator());
    if (ksm) {
      tbm = dynamic_cast<osgGA::TrackballManipulator*>(
          ksm->getCurrentMatrixManipulator());
    }
  }
  if (tbm) {
    tbm->setCenter(bs.center());
    if (zoom) tbm->setDistance(3 * bs.radius());
  } else {
    if (zoom) {
      osg::Vec3f tmp = center - eye;
      tmp.normalize();
      eye = bs.center() - tmp * 3 * bs.radius();
    } else {
      eye += bs.center() - center;
    }
    viewer->getCameraManipulator()->setByInverseMatrix(
        osg::Matrixd::lookAt(eye, bs.center(), up));
  }
}
}  // namespace gui
}  // namespace gepetto
