//
//  leaf-node-xyz.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert and Pierre Fernbach in april
//  2015. Copyright (c) 2015 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/leaf-node-xyzaxis.h>

#include <../src/internal/configuration.hh>
#include <osg/CullFace>
#include <osgText/Text>
#include <osgUtil/SmoothingVisitor>

namespace gepetto {
namespace viewer {
namespace {
// if rot corresponds to identity, the arrow is along Z.
inline osg::ShapeDrawableRefPtr arrowDrawable(
    const float& zshift, const float& lengthCyl, const float& radiusCyl,
    const float& lengthCon, const float& radiusCon, const osgQuat& rot) {
  /* Create cylinder */
  ::osg::CylinderRefPtr cylinder = new ::osg::Cylinder();
  cylinder->set(rot * osgVector3(0., 0., zshift + lengthCyl / 2.f), radiusCyl,
                lengthCyl);
  cylinder->setRotation(rot);
  /* Create cone */
  ::osg::ConeRefPtr cone = new ::osg::Cone();
  cone->set(rot * osgVector3(0., 0., zshift + lengthCyl + lengthCon / 2.f),
            lengthCon, radiusCon);
  cone->setRotation(rot);

  osg::ref_ptr<osg::CompositeShape> shape = new osg::CompositeShape;
  shape->addChild(cylinder);
  shape->addChild(cone);

  return new ::osg::ShapeDrawable(shape);
}

osg::ref_ptr<osgText::Text> label(const char* text,
                                  osg::ref_ptr<osgText::Font> font,
                                  const osgVector3& pos,
                                  const float& charSize) {
  osgVector4 black = osgVector4(0.f, 0.f, 0.f, 1.f);
  osg::ref_ptr<osgText::Text> l = new osgText::Text;
  l->setText(text);
  l->setFont(font);
  l->setCharacterSize(charSize);
  l->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
  l->setFontResolution(30, 30);
  l->setAlignment(osgText::Text::LEFT_CENTER);
  l->setAxisAlignment(osgText::Text::SCREEN);
  l->setColor(black);
  l->setPosition(pos);
  return l;
}

std::string labelGetText(const osgText::TextBase* t) {
  return t->getText().createUTF8EncodedString();
}

inline PropertyPtr_t axisColor(const std::string t, osg::ShapeDrawable* arrow) {
  return Vector4Property::create(t + "AxisColor",
                                 Vector4Property::getterFromMemberFunction(
                                     arrow, &osg::ShapeDrawable::getColor),
                                 Vector4Property::setterFromMemberFunction(
                                     arrow, &osg::ShapeDrawable::setColor));
}

inline PropertyPtr_t labelText(const std::string t, osgText::TextBase* text) {
  return StringProperty::create(
      t + "LabelText",
      StringProperty::Getter_t(boost::bind(labelGetText, text)),
      StringProperty::setterFromMemberFunction(text,
                                               &osgText::TextBase::setText));
}

inline PropertyPtr_t labelSize(const std::string t, osgText::TextBase* text) {
  return FloatProperty::create(
      t + "LabelSize",
      FloatProperty::getterFromMemberFunction(
          text, &osgText::TextBase::getCharacterHeight),
      FloatProperty::setterFromMemberFunction(
          text, &osgText::TextBase::setCharacterSize));
}
}  // namespace

/* Declaration of private function members */

void LeafNodeXYZAxis::init() {
  static osg::ref_ptr<osgText::Font> font = defaultFont();

  /* Create sphere object */
  sphere_ptr_ = new ::osg::Sphere();
  sphere_ptr_->setRadius(getRadius());
  /* Set ShapeDrawable */
  shape_drawable_ptr_ = new ::osg::ShapeDrawable(sphere_ptr_);

  /* Create Geode for adding ShapeDrawable */
  geode_ptr_ = new osg::Geode();
  geode_ptr_->addDrawable(shape_drawable_ptr_);
  if (sizeAxis_ > 0) {  // optimisation of memory consumption : doesn't create
                        // the axis instead of creating axis with size "0"
                        /* create the axis : */
    // float radiusCyl = (getRadius()/4.f) * getSizeAxis();
    float radiusCyl = getRadius() / 4.f;
    float lengthCyl = getSizeAxis();
    float labelShift1 = lengthCyl + getRadius() + 4 * radiusCyl + getRadius();
    float labelShift2 = 0;
    // float charSize = 3.f * radiusCyl;
    float charSize = 60;

    osgVector4 blue = osgVector4(0.f, 0.f, 1.f, 1.f);
    osgVector4 green = osgVector4(0.f, 1.f, 0.f, 1.f);
    osgVector4 red = osgVector4(1.f, 0.f, 0.f, 1.f);

    osg::ref_ptr<osgText::Text> lbl;
    osg::ShapeDrawableRefPtr arrow;

    /* X_AXIS */
    /* create drawable and add them to geode */
    arrow = arrowDrawable(getRadius(), lengthCyl, radiusCyl, 2.f * radiusCyl,
                          getRadius(),
                          osgQuat(0., ::osg::X_AXIS, ::osg::PI_2, ::osg::Y_AXIS,
                                  0., ::osg::Z_AXIS));
    arrow->setColor(red);

    lbl = label("", font, osgVector3(labelShift1, 0., labelShift2), charSize);

    addProperty(axisColor("X", arrow.get()));
    addProperty(labelText("X", lbl.get()));
    addProperty(labelSize("X", lbl.get()));

    geode_ptr_->addDrawable(arrow);
    geode_ptr_->addDrawable(lbl);

    /* Y_AXIS */
    /* create drawable and add them to geode */
    arrow = arrowDrawable(getRadius(), lengthCyl, radiusCyl, 2.f * radiusCyl,
                          getRadius(),
                          osgQuat(-::osg::PI_2, ::osg::X_AXIS, 0.,
                                  ::osg::Y_AXIS, 0., ::osg::Z_AXIS));
    arrow->setColor(green);

    lbl = label("", font, osgVector3(0., labelShift1, labelShift2), charSize);

    addProperty(axisColor("Y", arrow.get()));
    addProperty(labelText("Y", lbl.get()));
    addProperty(labelSize("Y", lbl.get()));

    geode_ptr_->addDrawable(arrow);
    geode_ptr_->addDrawable(lbl);

    /* Z_AXIS */
    /* create drawable and add them to geode */
    arrow = arrowDrawable(
        getRadius(), lengthCyl, radiusCyl, 2.f * radiusCyl, getRadius(),
        osgQuat(0., ::osg::X_AXIS, 0., ::osg::Y_AXIS, 0., ::osg::Z_AXIS));
    arrow->setColor(blue);

    lbl = label("", font, osgVector3(labelShift2, 0., labelShift1), charSize);

    addProperty(axisColor("Z", arrow.get()));
    addProperty(labelText("Z", lbl.get()));
    addProperty(labelSize("Z", lbl.get()));

    geode_ptr_->addDrawable(arrow);
    geode_ptr_->addDrawable(lbl);
  }  // if radius > 0 : create axis

  /* Create PositionAttitudeTransform */
  this->asQueue()->addChild(geode_ptr_);

  /* Allow transparency */
  geode_ptr_->getOrCreateStateSet()->setMode(GL_BLEND,
                                             ::osg::StateAttribute::ON);
  ;
}

LeafNodeXYZAxis::LeafNodeXYZAxis(const std::string& name,
                                 const osgVector4& color, float radiusCenter,
                                 float sizeAxis)
    : NodeDrawable(name) {
  setRadius(radiusCenter);
  setSizeAxis(sizeAxis);

  init();
  setColor(color);
}

LeafNodeXYZAxis::LeafNodeXYZAxis(const LeafNodeXYZAxis& other)
    : NodeDrawable(other) {
  init();
  // TODO
}

void LeafNodeXYZAxis::initWeakPtr(LeafNodeXYZAxisWeakPtr other_weak_ptr) {
  weak_ptr_ = other_weak_ptr;
}

/* End of declaration of private function members */

/* Declaration of protected function members */

LeafNodeXYZAxisPtr_t LeafNodeXYZAxis::create(const std::string& name,
                                             const osgVector4& color,
                                             float radiusCenter,
                                             float sizeAxis) {
  LeafNodeXYZAxisPtr_t shared_ptr(
      new LeafNodeXYZAxis(name, color, radiusCenter, sizeAxis));

  // Add reference to itself
  shared_ptr->initWeakPtr(shared_ptr);

  return shared_ptr;
}

LeafNodeXYZAxisPtr_t LeafNodeXYZAxis::create(const std::string& name,
                                             const osgVector4& color,
                                             float radiusCenter) {
  // const float& size = new float;
  //*size = 0;
  LeafNodeXYZAxisPtr_t shared_ptr(
      new LeafNodeXYZAxis(name, color, radiusCenter, 0));

  // Add reference to itself
  shared_ptr->initWeakPtr(shared_ptr);

  return shared_ptr;
}

LeafNodeXYZAxisPtr_t LeafNodeXYZAxis::createCopy(LeafNodeXYZAxisPtr_t other) {
  LeafNodeXYZAxisPtr_t shared_ptr(new LeafNodeXYZAxis(*other));

  // Add reference to itself
  shared_ptr->initWeakPtr(shared_ptr);

  return shared_ptr;
}

/* End of declaration of protected function members */

/* Declaration of public function members */

LeafNodeXYZAxisPtr_t LeafNodeXYZAxis::clone(void) const {
  return LeafNodeXYZAxis::createCopy(weak_ptr_.lock());
}

LeafNodeXYZAxisPtr_t LeafNodeXYZAxis::self(void) const {
  return weak_ptr_.lock();
}

void LeafNodeXYZAxis::setRadius(const float& radius) { radius_ = radius; }

float LeafNodeXYZAxis::getRadius() const { return radius_; }

void LeafNodeXYZAxis::setSizeAxis(const float& sizeAxis) {
  sizeAxis_ = sizeAxis;
}

float LeafNodeXYZAxis::getSizeAxis() const { return sizeAxis_; }

LeafNodeXYZAxis::~LeafNodeXYZAxis() {
  /* Proper deletion of all tree scene */
  geode_ptr_->removeDrawables(0, geode_ptr_->getNumDrawables());
  geode_ptr_ = NULL;
  weak_ptr_.reset();
}

/* End of declaration of public function members */

} /* namespace viewer */
} /* namespace gepetto */
