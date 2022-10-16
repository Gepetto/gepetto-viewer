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

#ifndef GEPETTO_GUI_PICK_HANDLER_HH
#define GEPETTO_GUI_PICK_HANDLER_HH

#include <gepetto/gui/fwd.hh>
// This include must be include before any other Qt include for GLDEBUGPROC
#include <QModelIndex>
#include <QObject>
#include <gepetto/gui/qt-osg-keyboard.hh>
#include <osg/ref_ptr>
#include <osgGA/GUIEventHandler>

namespace osgUtil {
class LineSegmentIntersector;
}

namespace gepetto {
namespace gui {
class PointIntersector;

class PickHandler : public osgGA::GUIEventHandler {
 public:
  PickHandler(OSGWidget* parent, WindowsManagerPtr_t wsm);

  virtual ~PickHandler();

  virtual bool handle(const osgGA::GUIEventAdapter& ea,
                      osgGA::GUIActionAdapter& aa);

  void getUsage(osg::ApplicationUsage& usage) const;

 private:
  typedef osg::ref_ptr<osgUtil::LineSegmentIntersector> LineSegmentIntersector;

  void computeLineIntersection(osgGA::GUIActionAdapter& aa, const float& x,
                               const float& y);

  /// \return the intersector that has an intersection (if one has).
  LineSegmentIntersector computeLineOrPointIntersection(
      osgGA::GUIActionAdapter& aa, const float& x, const float& y);

  void selectionNodeUnderCursor(osgGA::GUIActionAdapter& aa, const float& x,
                                const float& y, int modMask);

  void centerViewToMouse(osgGA::GUIActionAdapter& aa, const float& x,
                         const float& y);

  void setCameraToSelected(osgGA::GUIActionAdapter& aa, bool zoom);

  QtOsgKeyboard mapper_;
  WindowsManagerPtr_t wsm_;
  OSGWidget* parent_;
  bool pushed_;
  float lastX_, lastY_;

  LineSegmentIntersector lineIntersector_;
  osg::ref_ptr<PointIntersector> pointIntersector_;
};
}  // namespace gui
}  // namespace gepetto

#endif  // GEPETTO_GUI_PICK_HANDLER_HH
