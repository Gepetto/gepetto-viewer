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
class BackfaceDrawingProperty : public Property
{
  public:
    typedef osg::NodeRefPtr NodeRefPtr;

    bool hasReadAccess () const { return static_cast<bool>(node_); }
    bool hasWriteAccess() const { return static_cast<bool>(node_); }

    std::string type() { return details::property_type<bool>::to_string(); }

    BackfaceDrawingProperty () : Property("BackfaceDrawing") {}

    void node (const NodeRefPtr& node) { node_ = node; }
    NodeRefPtr node () const { return node_; }

    virtual QWidget* guiEditor ()
    {
      return details::buildEditor<bool>(this);
    }

  protected:
    bool impl_set(const bool& value);
    bool impl_get(      bool& value);

  private:
    NodeRefPtr node_;

};
} /* namespace viewer */
} /* namespace gepetto */

#endif /* GEPETTO_VIEWER_PROPERTIES_HH */
