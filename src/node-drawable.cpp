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
}
