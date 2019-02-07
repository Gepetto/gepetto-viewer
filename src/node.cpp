//
//  node.cpp
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#include <gepetto/viewer/node.h>

#include <climits>

#include <osg/Material>
#include <osg/LineWidth>
#include <osgFX/Outline>
#include <osgFX/Scribe>

#include <gepetto/viewer/window-manager.h>
#include <gepetto/viewer/node-visitor.h>

namespace graphics {
  namespace {
    const osg::StateSetRefPtr& getVisibleStateSet (const LightingMode& mode)
    {
      static osg::StateSetRefPtr ssOn, ssOff;
      switch (mode) {
        case LIGHT_INFLUENCE_ON:
          if (false && !ssOn) { // Disable because this is the default.
            ssOn = osg::StateSetRefPtr(new osg::StateSet());
            ssOn->setRenderBinToInherit();
            ssOn->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::ON  | ::osg::StateAttribute::PROTECTED);
            ssOn->setMode(GL_CULL_FACE , ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
            ssOn->setMode(GL_LIGHTING  , ::osg::StateAttribute::ON  | ::osg::StateAttribute::PROTECTED);
          }
          return ssOn;
        case LIGHT_INFLUENCE_OFF:
          if (!ssOff) {
            ssOff = osg::StateSetRefPtr(new osg::StateSet());
            ssOff->setRenderBinToInherit();
            ssOff->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::ON  | ::osg::StateAttribute::PROTECTED);
            ssOff->setMode(GL_CULL_FACE , ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
            ssOff->setMode(GL_LIGHTING  , ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
          }
          return ssOff;
        default:
          ASSERT(false, "LightingMode is not well defined");
          break;
      };
    }

    const osg::StateSetRefPtr& getAlwaysOnTopStateSet (const LightingMode& mode)
    {
      static osg::StateSetRefPtr ssOn, ssOff;
      switch (mode) {
        case LIGHT_INFLUENCE_ON:
          if (!ssOn) { // Disable because this is the default.
            ssOn = osg::StateSetRefPtr(new osg::StateSet());
            ssOn->setRenderBinDetails(INT_MAX, "DepthSortedBin");
            ssOn->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
            ssOn->setMode(GL_CULL_FACE , ::osg::StateAttribute::ON  | ::osg::StateAttribute::PROTECTED);
            ssOn->setMode(GL_LIGHTING  , ::osg::StateAttribute::ON  | ::osg::StateAttribute::PROTECTED);
          }
          return ssOn;
        case LIGHT_INFLUENCE_OFF:
          if (!ssOff) {
            ssOff = osg::StateSetRefPtr(new osg::StateSet());
            ssOff->setRenderBinDetails(INT_MAX, "DepthSortedBin");
            ssOff->setMode(GL_DEPTH_TEST, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
            ssOff->setMode(GL_CULL_FACE , ::osg::StateAttribute::ON  | ::osg::StateAttribute::PROTECTED);
            ssOff->setMode(GL_LIGHTING  , ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED);
          }
          return ssOff;
        default:
          ASSERT(false, "LightingMode is not well defined");
          break;
      };
    }

    const osg::StateSetRefPtr& getWireframeStateSet ()
    {
      static osg::StateSetRefPtr ss;
      if (!ss) {
        ss = osg::StateSetRefPtr(new osg::StateSet());

        /* Allowing wireframe mode */
        osg::PolygonModeRefPtr polygon_mode_ptr = new ::osg::PolygonMode;
        polygon_mode_ptr->setMode( ::osg::PolygonMode::FRONT_AND_BACK, ::osg::PolygonMode::LINE );
        polygon_mode_ptr->setDataVariance (osg::Object::STATIC);

        ::osg::MaterialRefPtr material_wireframe_ptr = new osg::Material;
        material_wireframe_ptr->setColorMode(osg::Material::DIFFUSE);
        material_wireframe_ptr->setDiffuse(osg::Material::FRONT_AND_BACK, osgVector4(1.,1.,1.,1.));
        material_wireframe_ptr->setDataVariance (osg::Object::STATIC);

        ss->setMode(GL_BLEND, ::osg::StateAttribute::OFF | ::osg::StateAttribute::PROTECTED ); // PROTECTED attribut allows wireframe node to not be influenced by alpha
        ss->setAttributeAndModes(polygon_mode_ptr, ::osg::StateAttribute::PROTECTED | ::osg::StateAttribute::ON );
        ss->setAttributeAndModes(material_wireframe_ptr, ::osg::StateAttribute::ON | ::osg::StateAttribute::PROTECTED );
      }
      return ss;
    }

    template <unsigned int state>
    const osg::StateSetRefPtr& getHighlightStateSet ()
    {
      static osg::StateSetRefPtr ss;
      if (state == 0) return ss;
      if (!ss) {
        ss = osg::StateSetRefPtr(new osg::StateSet());
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
          default:
            ASSERT(false, "HighlightState is not well defined");
            break;
        }
        material_switch_ptr->setDataVariance(osg::Object::STATIC);
        ss->setAttributeAndModes(material_switch_ptr, glModeValue);
      }
      return ss;
    }

    int getNodeVisibilityMode (Node* node) { return node->getVisibilityMode(); }
    void setNodeVisibilityMode (Node* node, const int& v) { node->setVisibilityMode((VisibilityMode)v); }

    int getNodeWireFrameMode (Node* node) { return node->getWireFrameMode(); }
    void setNodeWireFrameMode (Node* node, const int& v) { node->setWireFrameMode((WireFrameMode)v); }

    int getNodeLightingMode (Node* node) { return node->getLightingMode(); }
    void setNodeLightingMode (Node* node, const int& v) { node->setLightingMode((LightingMode)v); }

    void setNodeLandmark (Node* node, bool enable) {
      if (enable) node->addLandmark (0.05f);
      else        node->deleteLandmark();
    }

    int getNodeHighlightState (Node* node) { return (int)node->getHighlightState(); }
    void setNodeHighlightState (Node* node, const int& v) { node->setHighlightState(v); }

    MetaEnum* highlightStateEnum ()
    {
      static MetaEnum hs;
      if (hs.type.size() == 0) {
        hs.type = "HighlightState";
        hs.names .push_back ("None"  ); hs.values.push_back (0);
        hs.names .push_back ("1"     ); hs.values.push_back (1);
        hs.names .push_back ("2"     ); hs.values.push_back (2);
        hs.names .push_back ("3"     ); hs.values.push_back (3);
        hs.names .push_back ("4"     ); hs.values.push_back (4);
        hs.names .push_back ("5"     ); hs.values.push_back (5);
        hs.names .push_back ("6"     ); hs.values.push_back (6);
        hs.names .push_back ("7"     ); hs.values.push_back (7);
        hs.names .push_back ("8"     ); hs.values.push_back (8);
      }
      return &hs;
    }
  }
  using ::osg::Matrix;

  /* Declaration of private function members */
  void Node::init ()
  {
    /* Build local conformation
       <- wireframe_node_ptr_
       connected to the parent <- switch_node_ptr_                           <- auto_transform_ptr_ <- connection of children here
       <- normal_node_ptr_
    */

    switch_node_ptr_ = new ::osg::Group;
    hl_switch_node_ptr_ = new ::osg::Group;
    auto_transform_ptr_ = new ::osg::AutoTransform;
    static_auto_transform_ptr_ = new ::osg::MatrixTransform;

    switch_node_ptr_->setNodeMask(~NodeBit);
    switch_node_ptr_->setName (id_name_);
    wireframe_modes_.resize(2);
    wireframe_modes_[FILL]      = new ::osg::Group;
    wireframe_modes_[WIREFRAME] = new ::osg::Group;

    /* Building hierarchie */
    selected_wireframe_ = FILL;
    switch_node_ptr_->addChild(wireframe_modes_[FILL]);
    wireframe_modes_[FILL]->addChild(hl_switch_node_ptr_);
    wireframe_modes_[FILL]->setDataVariance (osg::Object::STATIC);

    // Setup highlight states
    highlight_nodes_.resize(9);
    selected_highlight_ = 0;
    highlight_enabled_ = true;
    highlight_nodes_[0] = transform_ptr_;
    for (unsigned int i = 1; i < 9; ++i) {
      highlight_nodes_[i] = setupHighlightState (i);
      highlight_nodes_[i]->addChild(transform_ptr_);
    }
    // setHighlightState(0);
    hl_switch_node_ptr_->addChild(highlight_nodes_[selected_highlight_]);

    auto_transform_ptr_->addChild(static_auto_transform_ptr_);

    wireframe_modes_[WIREFRAME]->setStateSet(getWireframeStateSet());
    wireframe_modes_[WIREFRAME]->setDataVariance (osg::Object::STATIC);
    geode_ptr_ = NULL;
    alpha_ = 0;

    visibilityMode_ = VISIBILITY_ON;
    lightingMode_   = LIGHT_INFLUENCE_ON;

    addProperty(
        EnumProperty::create("Highlight/State", highlightStateEnum(),
          EnumProperty::Getter_t(boost::bind(getNodeHighlightState, this)),
          EnumProperty::Setter_t(boost::bind(setNodeHighlightState, this, _1))));
    addProperty(
        BoolProperty::create("Highlight/Enable",
          BoolProperty::getterFromMemberFunction (this, &Node::getHighlightEnabled),
          BoolProperty::setterFromMemberFunction (this, &Node::setHighlightEnabled)));
    addProperty(
        EnumProperty::create("Visibility", visibilityModeEnum(),
          EnumProperty::Getter_t(boost::bind(getNodeVisibilityMode, this)),
          EnumProperty::Setter_t(boost::bind(setNodeVisibilityMode, this, _1))));
    addProperty(
        EnumProperty::create("WireframeMode", wireFrameModeEnum(),
          EnumProperty::Getter_t(boost::bind(getNodeWireFrameMode, this)),
          EnumProperty::Setter_t(boost::bind(setNodeWireFrameMode, this, _1))));
    addProperty(
        EnumProperty::create("LightingMode", lightingModeEnum(),
          EnumProperty::Getter_t(boost::bind(getNodeLightingMode, this)),
          EnumProperty::Setter_t(boost::bind(setNodeLightingMode, this, _1))));
    addProperty(
        BoolProperty::create("Landmark",
          BoolProperty::getterFromMemberFunction (this, &Node::hasLandmark),
          BoolProperty::Setter_t(boost::bind(setNodeLandmark, this, _1))));
    addProperty(
        StringProperty::create("Name",
          StringProperty::getterFromMemberFunction (this, &Node::getID),
          StringProperty::Setter_t()));
    addProperty(
        Vector4Property::create("Color",
          Vector4Property::Getter_t(),
          Vector4Property::setterFromMemberFunction(this, &Node::setColor)));
    addProperty(
        FloatProperty::create("Transparency",
          FloatProperty::getterFromMemberFunction(this, &Node::getAlpha),
          FloatProperty::setterFromMemberFunction(this, &Node::setAlpha)));
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

  const PropertyPtr_t& Node::property(const std::string& name) const
  {
    PropertyMap_t::const_iterator _prop = properties_.find(name);
    if (_prop == properties_.end())
      throw std::invalid_argument("Unknown property " + name);
    const PropertyPtr_t& prop = _prop->second;
    if (!prop)
      throw std::invalid_argument("Unknown property " + name);
    return prop;
  }

  bool Node::hasProperty(const std::string& name) const
  {
    PropertyMap_t::const_iterator _prop = properties_.find(name);
    return (_prop != properties_.end());
  }

  void Node::addProperty(const PropertyPtr_t& prop)
  {
    addProperty(prop->name(), prop);
  }

  void Node::addProperty(const std::string& name, const PropertyPtr_t& prop)
  {
    properties_[name] = prop;
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
    Matrix m(quat);
    m.setTrans(position);
    static_auto_transform_ptr_->setMatrix(m);
  }

  osgQuat Node::getStaticRotation() const
  {
    osgQuat q, so; osgVector3 t, s;
    static_auto_transform_ptr_->getMatrix().decompose(t, q, s, so);
    return q;
  }

  osgVector3 Node::getStaticPosition() const
  {
    osgQuat q, so; osgVector3 t, s;
    static_auto_transform_ptr_->getMatrix().decompose(t, q, s, so);
    return t;
  }

  osgVector3 Node::getScale() const
  {
    osgQuat q, so; osgVector3 t, s;
    static_auto_transform_ptr_->getMatrix().decompose(t, q, s, so);
    return s;
  }

  void Node::setScale(float scale)
  {
    osgQuat q, so; osgVector3 t, s;
    static_auto_transform_ptr_->getMatrix().decompose(t, q, s, so);
    Matrix m (q);
    m.setTrans(t);

    static_auto_transform_ptr_->setMatrix(Matrix::scale(scale, scale, scale) * m);
  }

 void Node::setScale(const osg::Vec3d &scale)
   {
    osgQuat q, so; osgVector3 t, s;
    static_auto_transform_ptr_->getMatrix().decompose(t, q, s, so);
    Matrix m (q);
    m.setTrans(t);

    static_auto_transform_ptr_->setMatrix(::osg::Matrix::scale(scale) * m);
   }

  void Node::setVisibilityMode (const VisibilityMode& mode)
  {
    visibilityMode_ = mode;
    switch (mode) {
      case VISIBILITY_ON:
        transform_ptr_->setNodeMask(0xffffffff);
        transform_ptr_->setStateSet (getVisibleStateSet(lightingMode_));
        break;
      case VISIBILITY_OFF:
        transform_ptr_->setNodeMask(0x0);
        transform_ptr_->setStateSet (getVisibleStateSet(lightingMode_));
        break;
      case ALWAYS_ON_TOP:
        transform_ptr_->setNodeMask(0xffffffff);
        transform_ptr_->setStateSet (getAlwaysOnTopStateSet(lightingMode_));
        break;

      default:
        ASSERT(false, "mode is not well defined");
        break;
    }
  }

  void Node::setLightingMode (const LightingMode& mode)
  {
    lightingMode_ = mode;
    switch (visibilityMode_) {
      case VISIBILITY_ON:
      case VISIBILITY_OFF:
        transform_ptr_->setStateSet (getVisibleStateSet(lightingMode_));
        break;
      case ALWAYS_ON_TOP:
        transform_ptr_->setStateSet (getAlwaysOnTopStateSet(lightingMode_));
        break;
      default:
        ASSERT(false, "mode is not well defined");
        break;
    }
  }

  LightingMode Node::getLightingMode () const
  {
    return lightingMode_;
  }

  void Node::setWireFrameMode (const WireFrameMode& mode)
  {
    if (mode == selected_wireframe_) return;
    bool fillSelected = (selected_wireframe_ == FILL     ) || (selected_wireframe_ == FILL_AND_WIREFRAME);
    bool wireSelected = (selected_wireframe_ == WIREFRAME) || (selected_wireframe_ == FILL_AND_WIREFRAME);
    // 0: keep as it is
    // 1: select
    // 2: deselect
    int selectFill = 0, selectWire = 0;
    switch (mode) {
    case FILL:
      if ( wireSelected) selectWire = 2;
      if (!fillSelected) selectFill = 1;
      break;
    case WIREFRAME:
      if (!wireSelected) selectWire = 1;
      if ( fillSelected) selectFill = 2;
      break;
    case FILL_AND_WIREFRAME:
      if (!wireSelected) selectWire = 1;
      if (!fillSelected) selectFill = 1;
      break;
    default:
      ASSERT(false, "WireFrameMode set with bad option");
      break;
    }
    switch (selectFill) {
      case 0: break;
      case 1:
              wireframe_modes_[FILL]->addChild(hl_switch_node_ptr_);
              switch_node_ptr_->addChild(wireframe_modes_[FILL]);
              break;
      case 2:
              wireframe_modes_[FILL]->removeChild(hl_switch_node_ptr_);
              switch_node_ptr_->removeChild(wireframe_modes_[FILL]);
              break;
      default:
              assert(false && "Wrong action");
    };
    switch (selectWire) {
      case 0: break;
      case 1:
              wireframe_modes_[WIREFRAME]->addChild(hl_switch_node_ptr_);
              switch_node_ptr_->addChild(wireframe_modes_[WIREFRAME]);
              break;
      case 2:
              wireframe_modes_[WIREFRAME]->removeChild(hl_switch_node_ptr_);
              switch_node_ptr_->removeChild(wireframe_modes_[WIREFRAME]);
              break;
      default:
              assert(false && "Wrong action");
    };
    selected_wireframe_ = mode;
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

    static_auto_transform_ptr_->removeChild(landmark_geode_ptr_);
    landmark_geode_ptr_ = new osg::Geode();
    landmark_geode_ptr_->addDrawable(geom_ptr);

    //set Landmark as ALWAYS ON TOP
    landmark_geode_ptr_->setNodeMask(0xffffffff);
    landmark_geode_ptr_->setStateSet (getAlwaysOnTopStateSet (LIGHT_INFLUENCE_OFF));

    static_auto_transform_ptr_->addChild(landmark_geode_ptr_);
  }

  bool Node::hasLandmark() const
  {
    return landmark_geode_ptr_;
  }

  void Node::deleteLandmark()
  {
    static_auto_transform_ptr_->removeChild(landmark_geode_ptr_);
    landmark_geode_ptr_.release();
  }

  ::osg::Group* Node::setupHighlightState (unsigned int state)
  {
    osg::StateSetRefPtr ss;
    switch (state) {
      case 1: /// collision
        ss = getHighlightStateSet<1> ();
        break;
      case 2: /// selection
        ss = getHighlightStateSet<2> ();
        break;
      case 3: /// selection
        ss = getHighlightStateSet<3> ();
        break;
      case 4: /// selection
        ss = getHighlightStateSet<4> ();
        break;
      case 5: /// selection
        ss = getHighlightStateSet<5> ();
        break;
      case 6: /// selection
        ss = getHighlightStateSet<6> ();
        break;
      case 7: /// Selection through osgFX::Outline
        {
          osgFX::Outline* outline = new osgFX::Outline;

          outline->setWidth(8);
          outline->setColor(osg::Vec4(1,1,0,1));
          outline->setDataVariance(osg::Object::STATIC);
          return outline;
        }
        break;
      case 8: /// Selection through osgFX::Outline
        {
          osgFX::Scribe* scribe = new osgFX::Scribe;

          scribe->setWireframeLineWidth(1);
          scribe->setWireframeColor(osg::Vec4(1,1,0,1));
          scribe->setDataVariance(osg::Object::STATIC);
          return scribe;
        }
        break;
      case 0:
      default:
        break;
    }
    osg::Group* node = new ::osg::Group;
    node->setStateSet(ss);
    node->setDataVariance(osg::Object::STATIC);
    return node;
  }

  void Node::setHighlightState (unsigned int state)
  {
    if (!highlight_enabled_) return;
    if (state != selected_highlight_
        && state < highlight_nodes_.size()) {
      hl_switch_node_ptr_->replaceChild(highlight_nodes_[selected_highlight_],
                                        highlight_nodes_[state]);
      // Update the child
      selected_highlight_ = state;
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
    /* deleting the top most node (switch_node_ptr_) will delete everything else.
     * Loop over the parents of switch_node_ptr_ and remove references to it.
     */
    typedef ::osg::Node::ParentList PL_t;
    PL_t parents = switch_node_ptr_->getParents();
    for (PL_t::const_iterator _p = parents.begin(); _p != parents.end(); ++_p)
      (*_p)->removeChild(switch_node_ptr_);
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
