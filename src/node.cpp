//
//  node.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/node.h>

#include <gepetto/viewer/node-visitor.h>
#include <osg/Material>
#include <osg/LineWidth>
#include <osgFX/Outline>
#include <osgFX/Scribe>
#include <climits>

namespace graphics {

  bool Node::firstTime_ = true;
  ::osg::NodeRefPtr Node::arrow_ = 0;
  ::osg::NodeRefPtr Node::circle_ = 0;

  /* Declaration of private function members */
  void Node::init ()
  {
    if (firstTime_) {
      const float factor = 0.1f;
      ::osg::GroupRefPtr group = new ::osg::Group;
      ::osg::GeodeRefPtr cylinder = new osg::Geode;
      cylinder->addDrawable (new osg::ShapeDrawable (new osg::Cylinder (osg::Vec3(0,0,0), 1*factor, 4*factor)));
      group->addChild (cylinder);
      ::osg::GeodeRefPtr cone = new osg::Geode;
      cone->addDrawable (new osg::ShapeDrawable (new osg::Cone (osg::Vec3(0,0,0), 1.8f*factor, 2.0f*factor)));
      ::osg::PositionAttitudeTransformRefPtr cone_transform = new ::osg::PositionAttitudeTransform;
      cone_transform->addChild (cone);
      cone_transform->setPosition (osg::Vec3 (0,0,2*factor));
      group->addChild (cone_transform);
      arrow_ = group;
      ::osg::GeometryRefPtr circle = new ::osg::Geometry;
      ::osg::ref_ptr< ::osg::Vec3Array> v = new ::osg::Vec3Array;
      const float radius = 1; const int POLYGON_SIZE = 256;
      float theta, px, py;
      for (int i = 1; i <= POLYGON_SIZE; ++i) {
        theta = 2.f * (float)osg::PI / (float)(POLYGON_SIZE * i);
        px = radius * cosf (theta);
        py = radius * sinf (theta);
        v->push_back (osg::Vec3(px,py,0));
      }
      circle->setVertexArray (v.get());
      circle->addPrimitiveSet (
          new ::osg::DrawArrays (::osg::PrimitiveSet::LINE_LOOP, 0, POLYGON_SIZE));
      ::osg::StateSetRefPtr stateset = new ::osg::StateSet;
      ::osg::LineWidthRefPtr linewidth = new ::osg::LineWidth;
      linewidth->setWidth (2.0f);
      stateset->setAttributeAndModes (linewidth, osg::StateAttribute::ON);
      circle->setStateSet (stateset);
      ::osg::GeodeRefPtr circleNode = new ::osg::Geode;
      circleNode->addDrawable (circle.get());
      circle_ = circleNode;
      firstTime_ = false;
    }

    /* Build local conformation
       <- wireframe_node_ptr_
       connected to the parent <- switch_node_ptr_                           <- auto_transform_ptr_ <- connection of children here
       <- normal_node_ptr_
    */

    auto_transform_ptr_ = new ::osg::AutoTransform;
    static_auto_transform_ptr_ = new ::osg::AutoTransform;
    arrows_transform_ptr_ = new ::osg::PositionAttitudeTransform;
    switch_node_ptr_ = new ::osg::Switch;
    switch_node_ptr_->setName (id_name_);
    hl_switch_node_ptr_ = new ::osg::Switch;

    wireframe_node_ptr_ = new ::osg::Group;
    normal_node_ptr_ = new ::osg::Group;

    /* Building hierarchie */
    switch_node_ptr_->addChild(normal_node_ptr_, true);
    switch_node_ptr_->addChild(wireframe_node_ptr_, false);

    switch_node_ptr_->getOrCreateStateSet()->setDataVariance(::osg::Object::DYNAMIC);

    wireframe_node_ptr_->addChild(hl_switch_node_ptr_);
    normal_node_ptr_->addChild(hl_switch_node_ptr_);

    ::osg::GroupRefPtr g = setupHighlightState (0);
    hl_switch_node_ptr_->addChild (g, true);
    g->addChild (auto_transform_ptr_);
    for (unsigned int i = 1; i < 9; ++i) {
      ::osg::GroupRefPtr g = setupHighlightState (i);
      hl_switch_node_ptr_->addChild (g, false);
      g->addChild (auto_transform_ptr_);
    }

    auto_transform_ptr_->addChild(static_auto_transform_ptr_);

    /* Allowing wireframe mode */
    polygon_mode_ptr_ = new ::osg::PolygonMode;
    polygon_mode_ptr_->setMode( ::osg::PolygonMode::FRONT_AND_BACK, ::osg::PolygonMode::LINE );

    ::osg::MaterialRefPtr material_wireframe_ptr = new osg::Material;
    material_wireframe_ptr->setColorMode(osg::Material::DIFFUSE);
    material_wireframe_ptr->setDiffuse(osg::Material::FRONT_AND_BACK, osgVector4(1.,1.,1.,1.));

    wireframe_node_ptr_->getOrCreateStateSet()->setAttributeAndModes(polygon_mode_ptr_, ::osg::StateAttribute::PROTECTED | ::osg::StateAttribute::ON );
    wireframe_node_ptr_->getOrCreateStateSet()->setMode(GL_BLEND, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED ); // PROTECTED attribut allows wireframe node to not be influenced by alpha
    wireframe_node_ptr_->getOrCreateStateSet()->setAttributeAndModes(material_wireframe_ptr, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED );
    geode_ptr_ = NULL;
    alpha_ = 0;
  }

  Node::Node (const std::string& name) :
    id_name_(name)
  {
    init();
  }

  Node::Node (const Node &other) :
    id_name_(other.getID())
  {
    init();
  }

  void Node::applyConfiguration (const osgVector3 & position, const osgQuat & quat)
  {
    /* Update position */
    auto_transform_ptr_->setPosition(position);

    /* Update attitude */
    auto_transform_ptr_->setRotation(quat);

  }

  void Node::setStaticTransform(const osgVector3 & position, const osgQuat & quat)
  {
    /* Update position */
    static_auto_transform_ptr_->setPosition(position);

    /* Update attitude */
    static_auto_transform_ptr_->setRotation(quat);
  }

  osgQuat Node::getStaticRotation() const
  {
    return static_auto_transform_ptr_->getRotation();
  }

  osgVector3 Node::getStaticPosition() const
  {
    return static_auto_transform_ptr_->getPosition();
  }

  void Node::setScale(float scale)
  {
    static_auto_transform_ptr_->setScale(scale);
  }

 void Node::setScale(const osg::Vec3d &scale)
   {
     double lmin=scale._v[0];
     if (lmin>scale._v[1])
       lmin=scale._v[1];
     if (lmin>scale._v[2])
       lmin=scale._v[2];
 
     if(lmin<static_auto_transform_ptr_->getMinimumScale())
       static_auto_transform_ptr_->setMinimumScale(lmin);
        
     static_auto_transform_ptr_->setScale(scale);
   }

  void Node::setVisibilityMode (const VisibilityMode& mode)
  {
    switch (mode) {
    case VISIBILITY_ON:
      auto_transform_ptr_->setNodeMask(0xffffffff);
      auto_transform_ptr_->getOrCreateStateSet()->setRenderBinToInherit();
      auto_transform_ptr_->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED);
      auto_transform_ptr_->getOrCreateStateSet()->setMode(GL_CULL_FACE,::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
      break;
    case VISIBILITY_OFF:
      auto_transform_ptr_->setNodeMask(0x0);
      auto_transform_ptr_->getOrCreateStateSet()->setRenderBinToInherit();
      auto_transform_ptr_->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED);
      auto_transform_ptr_->getOrCreateStateSet()->setMode(GL_CULL_FACE,::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
      break;
    case ALWAYS_ON_TOP:
      auto_transform_ptr_->setNodeMask(0xffffffff);
      auto_transform_ptr_->getOrCreateStateSet()->setRenderBinDetails(INT_MAX, "DepthSortedBin");
      auto_transform_ptr_->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
      auto_transform_ptr_->getOrCreateStateSet()->setMode(GL_CULL_FACE, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED );
      break;

    default:
      ASSERT(false, "mode is not well defined");
      break;
    }
  }

  void Node::setLightingMode (const LightingMode& mode)
  {
    if(mode == LIGHT_INFLUENCE_ON)
      auto_transform_ptr_->getOrCreateStateSet()->setMode(GL_LIGHTING, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED);
    else
      auto_transform_ptr_->getOrCreateStateSet()->setMode(GL_LIGHTING, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
  }

  void Node::setWireFrameMode (const WireFrameMode& mode)
  {
    switch (mode) {
    case FILL:
      switch_node_ptr_->setValue (0, true);
      switch_node_ptr_->setValue (1, false);
      break;

    case WIREFRAME:
      switch_node_ptr_->setValue (0, false);
      switch_node_ptr_->setValue (1, true);
      break;

    case FILL_AND_WIREFRAME:
      switch_node_ptr_->setValue (0, true);
      switch_node_ptr_->setValue (1, true);
      break;

    default:
      ASSERT(false, "WireFrameMode set with bad option");
      break;
    }
  }

  void Node::selectArrows (int arrows)
  {
    auto_transform_ptr_->removeChild(arrows_transform_ptr_);
    arrows_transform_ptr_ = new ::osg::PositionAttitudeTransform;
    auto_transform_ptr_->addChild(arrows_transform_ptr_);
    float radius = static_auto_transform_ptr_->getBound ().radius();
    ::osg::PositionAttitudeTransformRefPtr axis = 0;
    if (arrows & TRANS_X) {
      axis = new ::osg::PositionAttitudeTransform;
      axis->setPosition (osg::Vec3(radius, 0, 0));
      axis->setAttitude (osg::Quat(osg::PI_2, osg::Vec3(0, 1, 0)));
      axis->setScale (osg::Vec3 (radius, radius, radius));
      axis->addChild (arrow_);
      arrows_transform_ptr_->addChild (axis);
    }
    if (arrows & TRANS_Y) {
      axis = new ::osg::PositionAttitudeTransform;
      axis->setPosition (osg::Vec3(0, radius, 0));
      axis->setAttitude (osg::Quat(-osg::PI_2, osg::Vec3(1, 0, 0)));
      axis->setScale (osg::Vec3 (radius, radius, radius));
      axis->addChild (arrow_);
      arrows_transform_ptr_->addChild (axis);
    }
    if (arrows & TRANS_Z) {
      axis = new ::osg::PositionAttitudeTransform;
      axis->setPosition (osg::Vec3(0, 0, radius));
      axis->setScale (osg::Vec3 (radius, radius, radius));
      axis->addChild (arrow_);
      arrows_transform_ptr_->addChild (axis);
    }
    if (arrows & ROT_X) {
      axis = new ::osg::PositionAttitudeTransform;
      axis->setScale (osg::Vec3 (radius, radius, radius));
      axis->setAttitude (osg::Quat(osg::PI_2, osg::Vec3(0, 1, 0)));
      axis->addChild (circle_);
      arrows_transform_ptr_->addChild (axis);
    }
    if (arrows & ROT_Y) {
      axis = new ::osg::PositionAttitudeTransform;
      axis->setScale (osg::Vec3 (radius, radius, radius));
      axis->setAttitude (osg::Quat(-osg::PI_2, osg::Vec3(1, 0, 0)));
      axis->addChild (circle_);
      arrows_transform_ptr_->addChild (axis);
    }
    if (arrows & ROT_Z) {
      axis = new ::osg::PositionAttitudeTransform;
      axis->setScale (osg::Vec3 (radius, radius, radius));
      axis->addChild (circle_);
      arrows_transform_ptr_->addChild (axis);
    }
    setArrowsVisibility (VISIBILITY_OFF);
  }

  void Node::setArrowsVisibility (const VisibilityMode& mode)
  {
    switch (mode) {
    case VISIBILITY_ON:
      arrows_transform_ptr_->setNodeMask(0xffffffff);
      arrows_transform_ptr_->getOrCreateStateSet()->setRenderBinToInherit();
      arrows_transform_ptr_->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED);
      arrows_transform_ptr_->getOrCreateStateSet()->setMode(GL_CULL_FACE,::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
      break;
    case VISIBILITY_OFF:
      arrows_transform_ptr_->setNodeMask(0x0);
      arrows_transform_ptr_->getOrCreateStateSet()->setRenderBinToInherit();
      arrows_transform_ptr_->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED);
      arrows_transform_ptr_->getOrCreateStateSet()->setMode(GL_CULL_FACE,::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
      break;
    case ALWAYS_ON_TOP:
      arrows_transform_ptr_->setNodeMask(0xffffffff);
      arrows_transform_ptr_->getOrCreateStateSet()->setRenderBinDetails(INT_MAX, "DepthSortedBin");
      arrows_transform_ptr_->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
      arrows_transform_ptr_->getOrCreateStateSet()->setMode(GL_CULL_FACE, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED );
      break;

    default:
      ASSERT(false, "mode is not well defined");
      break;
    }
  }

  void Node::addLandmark(const float& size)
  {
    ::osg::GeometryRefPtr geom_ptr = new ::osg::Geometry();

    /* Define points of the beam */
    ::osg::Vec3ArrayRefPtr points_ptr = new ::osg::Vec3Array(6);
    points_ptr->at(0) = osgVector3(0.,0.,0.);
    points_ptr->at(1) = osgVector3(size,0.,0.);
    points_ptr->at(2) = osgVector3(0.,0.,0.);
    points_ptr->at(3) = osgVector3(0.,size,0.);
    points_ptr->at(4) = osgVector3(0.,0.,0.);
    points_ptr->at(5) = osgVector3(0.,0.,size);


    /* Define the color */
    ::osg::Vec4ArrayRefPtr color_ptr = new ::osg::Vec4Array(3);
    color_ptr->at(0) = osgVector4(1.,0.,0.,1.);
    color_ptr->at(1) = osgVector4(0.,1.,0.,1.);
    color_ptr->at(2) = osgVector4(0.,0.,1.,1.);

    geom_ptr->setVertexArray(points_ptr.get());
    geom_ptr->setColorArray(color_ptr   .get());
    geom_ptr->setColorBinding(::osg::Geometry::BIND_PER_PRIMITIVE_SET);
    geom_ptr->addPrimitiveSet(new osg::DrawArrays(GL_LINES,0,2));
    geom_ptr->addPrimitiveSet(new osg::DrawArrays(GL_LINES,2,2));
    geom_ptr->addPrimitiveSet(new osg::DrawArrays(GL_LINES,4,2));

    landmark_geode_ptr_ = new osg::Geode();
    landmark_geode_ptr_->addDrawable(geom_ptr);

    //set Landmark as ALWAYS ON TOP
    landmark_geode_ptr_->setNodeMask(0xffffffff);
    landmark_geode_ptr_->getOrCreateStateSet()->setRenderBinDetails(INT_MAX, "DepthSortedBin");
    landmark_geode_ptr_->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
    landmark_geode_ptr_->getOrCreateStateSet()->setMode(GL_CULL_FACE, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED );
    landmark_geode_ptr_->getOrCreateStateSet()->setMode(GL_LIGHTING, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
    /*auto_transform_ptr_->setNodeMask(0xffffffff);
    auto_transform_ptr_->getOrCreateStateSet()->setRenderBinToInherit();
    auto_transform_ptr_->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED);
    auto_transform_ptr_->getOrCreateStateSet()->setMode(GL_CULL_FACE,::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
    landmark_geode_ptr_->getOrCreateStateSet()->setMode(GL_LIGHTING, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);*/


    static_auto_transform_ptr_->addChild(landmark_geode_ptr_);
  }

  void Node::deleteLandmark()
  {
    static_auto_transform_ptr_->removeChild(landmark_geode_ptr_);
    landmark_geode_ptr_.release();
  }

  ::osg::Group* Node::setupHighlightState (unsigned int state)
  {
    ::osg::MaterialRefPtr material_switch_ptr = new osg::Material;
    int glModeValue = ::osg::StateAttribute::INHERIT;
    /// Some color codes are taken from
    /// http://devernay.free.fr/cours/opengl/materials.html
    switch (state) {
      case 1: /// collision
        glModeValue = ::osg::StateAttribute::ON | ::osg::StateAttribute::OVERRIDE;
        material_switch_ptr->setColorMode (osg::Material::AMBIENT);
        material_switch_ptr->setDiffuse  (osg::Material::FRONT_AND_BACK, osgVector4(1.0f,0.f,0.f,1.f));
        material_switch_ptr->setTransparency(osg::Material::FRONT_AND_BACK, 0.5f);
        break;
      case 2: /// selection
        glModeValue = ::osg::StateAttribute::ON | ::osg::StateAttribute::OVERRIDE | ::osg::StateAttribute::PROTECTED;
        /// Blue
        material_switch_ptr->setAmbient  (osg::Material::FRONT_AND_BACK, osgVector4(0.f,0.f,0.f,1.f));
        material_switch_ptr->setDiffuse  (osg::Material::FRONT_AND_BACK, osgVector4(0.f,0.f,1.f,1.f));
        material_switch_ptr->setSpecular (osg::Material::FRONT_AND_BACK, osgVector4(0.6f,0.6f,0.7f,1.f));
        material_switch_ptr->setShininess(osg::Material::FRONT_AND_BACK, 128.f);
        break;
      case 3: /// selection
        glModeValue = ::osg::StateAttribute::ON | ::osg::StateAttribute::OVERRIDE | ::osg::StateAttribute::PROTECTED;
        /// Red
        material_switch_ptr->setAmbient  (osg::Material::FRONT_AND_BACK, osgVector4(0.f,0.f,0.f,1.f));
        material_switch_ptr->setDiffuse  (osg::Material::FRONT_AND_BACK, osgVector4(1.0f,0.f,0.f,1.f));
        material_switch_ptr->setSpecular (osg::Material::FRONT_AND_BACK, osgVector4(0.7f,0.6f,0.6f,1.f));
        material_switch_ptr->setShininess(osg::Material::FRONT_AND_BACK, 96.f);
        break;
      case 4: /// selection
        glModeValue = ::osg::StateAttribute::ON | ::osg::StateAttribute::OVERRIDE | ::osg::StateAttribute::PROTECTED;
        /// Red plastic
        material_switch_ptr->setAmbient  (osg::Material::FRONT_AND_BACK, osgVector4(0.f,0.f,0.f,1.f));
        material_switch_ptr->setDiffuse  (osg::Material::FRONT_AND_BACK, osgVector4(0.5f,0.f,0.f,1.f));
        material_switch_ptr->setSpecular (osg::Material::FRONT_AND_BACK, osgVector4(0.7f,0.6f,0.6f,1.f));
        material_switch_ptr->setShininess(osg::Material::FRONT_AND_BACK, 32.f);
        break;
      case 5: /// selection
        glModeValue = ::osg::StateAttribute::ON | ::osg::StateAttribute::OVERRIDE | ::osg::StateAttribute::PROTECTED;
        /// Bronze
        material_switch_ptr->setAmbient  (osg::Material::FRONT_AND_BACK, osgVector4(0.2125f,0.1275f,0.054f,1.f));
        material_switch_ptr->setDiffuse  (osg::Material::FRONT_AND_BACK, osgVector4(0.714f,0.4284f,0.18144f,1.f));
        material_switch_ptr->setSpecular (osg::Material::FRONT_AND_BACK, osgVector4(0.393548f,0.271906f,0.166721f,1.f));
        material_switch_ptr->setShininess(osg::Material::FRONT_AND_BACK, 26.f);
        break;
      case 6: /// selection
        glModeValue = ::osg::StateAttribute::ON | ::osg::StateAttribute::OVERRIDE | ::osg::StateAttribute::PROTECTED;
        /// Red rubber
        material_switch_ptr->setAmbient  (osg::Material::FRONT_AND_BACK, osgVector4(0.05f,0.f,0.f,1.f));
        material_switch_ptr->setDiffuse  (osg::Material::FRONT_AND_BACK, osgVector4(0.5f,0.5f,0.4f,1.f));
        material_switch_ptr->setSpecular (osg::Material::FRONT_AND_BACK, osgVector4(0.7f,0.04f,0.04f,1.f));
        material_switch_ptr->setShininess(osg::Material::FRONT_AND_BACK, 10.f);
        break;
      case 7: /// Selection through osgFX::Outline
        {
          osgFX::Outline* outline = new osgFX::Outline;

          outline->setWidth(8);
          outline->setColor(osg::Vec4(1,1,0,1));
          return outline;
        }
        break;
      case 8: /// Selection through osgFX::Outline
        {
          osgFX::Scribe* scribe = new osgFX::Scribe;

          scribe->setWireframeLineWidth(1);
          scribe->setWireframeColor(osg::Vec4(1,1,0,1));
          return scribe;
        }
        break;
      case 0:
      default:
        break;
    }
    osg::Group* node = new ::osg::Group;
    node->getOrCreateStateSet()->setAttributeAndModes(material_switch_ptr, glModeValue);
    return node;
  }

  void Node::setHighlightState (unsigned int state)
  {
    if (state < hl_switch_node_ptr_->getNumChildren ()) {
      hl_switch_node_ptr_->setAllChildrenOff ();
      hl_switch_node_ptr_->setSingleChildOn (state);
    }
  }

  void Node::setAlpha (const float& alpha)
  {
    if (geode_ptr_.get() == NULL)
      {
	std::cout << "You must initialize a Geode on " << id_name_ << " to use Alpha" << std::endl;
	return ;
      }
    osg::StateSet* ss = geode_ptr_.get()->getStateSet();
    if (ss)
      {
	alpha_ = alpha;
	osg::Material *mat;
	if (ss->getAttribute(osg::StateAttribute::MATERIAL))
	  mat = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
	else
	  {
	    mat = new osg::Material;
	    ss->setAttribute(mat, osg::StateAttribute::OFF);
	  }
	mat->setTransparency(osg::Material::FRONT_AND_BACK, alpha);
	if (alpha_ > 0)
	  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	else
	  ss->setRenderingHint(osg::StateSet::DEFAULT_BIN);
      }
  }

  float Node::getAlpha() const
  {
    return alpha_;
  }

  Node::~Node ()
  {
    /* Proper deletion */
    normal_node_ptr_->removeChild (auto_transform_ptr_);
    wireframe_node_ptr_->removeChild (auto_transform_ptr_);
    auto_transform_ptr_ = NULL;

    polygon_mode_ptr_ = NULL;
    switch_node_ptr_->removeChild (wireframe_node_ptr_);
    wireframe_node_ptr_ = NULL;

    switch_node_ptr_->removeChild (normal_node_ptr_);
    normal_node_ptr_ = NULL;
  }
  
  std::pair<osgVector3, osgQuat> Node::getGlobalTransform() const
  {
    return std::make_pair(auto_transform_ptr_->getPosition(),auto_transform_ptr_->getRotation());
  }

  void Node::traverse (NodeVisitor& /*visitor*/) {
  }

  osg::ref_ptr<osg::Node> Node::getOsgNode() const
  {
    return geode_ptr_.get();
  }

  /* End of declaration of public function members */


} /* namespace graphics */
