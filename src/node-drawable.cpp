// Copyright (c) 2018, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)

#include <gepetto/viewer/node-drawable.h>

#include <osg/Version>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

namespace gepetto {
namespace viewer {
  void NodeDrawable::init ()
  {
    addProperty(Vector4Property::create("Color",
          Vector4Property::getterFromMemberFunction(this, &NodeDrawable::getColor),
          Vector4Property::setterFromMemberFunction(this, &NodeDrawable::setColor)));
  }

  void NodeDrawable::setColor(const osgVector4 &color)
  {
    shape_drawable_ptr_->setColor(color);
    redrawShape ();
  }

  void NodeDrawable::redrawShape ()
  {
#if OSG_VERSION_GREATER_OR_EQUAL(3, 5, 6)
    shape_drawable_ptr_->build();
#else
    shape_drawable_ptr_->dirtyDisplayList();
    shape_drawable_ptr_->dirtyBound();
#endif
    setDirty();
  }

  osgVector4 NodeDrawable::getColor() const
  {
    return shape_drawable_ptr_->getColor();
  }

  void NodeDrawable::setTexture(const std::string& image_path)
  {
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setDataVariance(osg::Object::DYNAMIC);
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(image_path);
    if (!image)
    {
      std::cerr << " couldn't find texture " << image_path << ", quiting." << std::endl;
      return;
    }
    texture->setImage(image);
    geode_ptr_->getStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
    setDirty();
  }
} /* namespace viewer */
}
