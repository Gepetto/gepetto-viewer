// Copyright (c) 2018, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)

#include <gepetto/viewer/node-drawable.h>

namespace graphics {
  void NodeDrawable::init ()
  {
    addProperty(Vector4Property::create("Color",
          Vector4Property::getterFromMemberFunction(this, &NodeDrawable::getColor),
          Vector4Property::setterFromMemberFunction(this, &NodeDrawable::setColor)));
  }

  void NodeDrawable::setColor(const osgVector4 &color)
  {
    shape_drawable_ptr_->setColor(color);
#ifdef OSG_3_5_6_OR_LATER
    shape_drawable_ptr_->build();
#else
    shape_drawable_ptr_->dirtyDisplayList();
    shape_drawable_ptr_->dirtyBound();
#endif
  }

  osgVector4 NodeDrawable::getColor() const
  {
    return shape_drawable_ptr_->getColor();
  }
}
