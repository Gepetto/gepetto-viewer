// Copyright (c) 2020, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)

#include <gepetto/viewer/properties.h>

#include <osg/CullFace>
#include <osg/LightModel>

namespace gepetto {
namespace viewer {
bool BackfaceDrawingProperty::impl_get(bool& value) {
  if (!hasReadAccess()) {
    invalidGet();
    return false;
  }
  value = static_cast<bool>(stateSet_->getMode(GL_CULL_FACE) &
                            osg::StateAttribute::ON);
  return true;
}

bool BackfaceDrawingProperty::impl_set(const bool& on) {
  if (!hasWriteAccess()) {
    invalidSet();
    return false;
  }

  stateSet_->setMode(GL_CULL_FACE,
                     (on ? osg::StateAttribute::ON : osg::StateAttribute::OFF));

  if (on) {
    osg::LightModel* ltModel = new osg::LightModel;
    ltModel->setTwoSided(on);
    stateSet_->setAttribute(ltModel);
    stateSet_->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
  } else {
    stateSet_->removeAttribute(osg::StateAttribute::LIGHTMODEL);
    stateSet_->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
  }
  return true;
}

} /* namespace viewer */
} /* namespace gepetto */
