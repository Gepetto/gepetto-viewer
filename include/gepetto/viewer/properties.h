///
///  properties.h
///  gepetto-viewer
///
///  Copyright (c) 2020 LAAS-CNRS. All rights reserved.
///  \author Joseph Mirabel
///

#ifndef GEPETTO_VIEWER_PROPERTIES_HH
#define GEPETTO_VIEWER_PROPERTIES_HH

#include <gepetto/viewer/node-property.h>

namespace gepetto {
namespace viewer {
class BackfaceDrawingProperty : public Property {
 public:
  typedef osg::StateSetRefPtr StateSetRefPtr;

  bool hasReadAccess() const { return static_cast<bool>(stateSet_); }
  bool hasWriteAccess() const { return static_cast<bool>(stateSet_); }

  std::string type() { return details::property_type<bool>::to_string(); }

  BackfaceDrawingProperty() : Property("BackfaceDrawing") {}

  void stateSet(const StateSetRefPtr& stateSet) { stateSet_ = stateSet; }
  StateSetRefPtr stateSet() const { return stateSet_; }

  virtual QWidget* guiEditor() { return details::buildEditor<bool>(this); }

 protected:
  bool impl_set(const bool& value);
  bool impl_get(bool& value);

 private:
  osg::StateSetRefPtr stateSet_;
};
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_PROPERTIES_HH */
