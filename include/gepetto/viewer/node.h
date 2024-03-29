//
//  node.h
//  gepetto-viewer
//
//  Created by Justin Carpentier, Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef GEPETTO_VIEWER_NODE_HH
#define GEPETTO_VIEWER_NODE_HH

#include <gepetto/viewer/config-osg.h>
#include <gepetto/viewer/fwd.h>
#include <gepetto/viewer/node-property.h>
#include <gepetto/viewer/node-visitor.h>

#include <iostream>

namespace gepetto {
namespace viewer {

enum { VisibilityBit = 1 << 0, IntersectionBit = 1 << 1, NodeBit = 1 << 2 };

/// Abstract base class of 3D objects in a scene.
class Node : public Properties {
 private:
  friend struct NodeTest;

  std::string id_name_;  // automoatic id generated by the program
  bool dirty_;

  /** PositionAttitudeTransform related to the global configuration */
  osg::MatrixTransformRefPtr transform_ptr_;
  RangedStoredPropertyTpl<osgVector3, float> scale_;
  osg::Matrixf Ms_;
  StoredPropertyTpl<Configuration> M_;

  /** Associated switch node */
  /** TODO: The use of multiswitch may be better */
  osg::GroupRefPtr switch_node_ptr_;
  WireFrameMode selected_wireframe_;
  std::vector< ::osg::GroupRefPtr> wireframe_modes_;

  osg::GroupRefPtr hl_switch_node_ptr_;
  std::size_t selected_highlight_;
  bool highlight_enabled_;
  std::vector< ::osg::GroupRefPtr> highlight_nodes_;

  VisibilityMode visibilityMode_;
  LightingMode lightingMode_;

  /** Initialization function */
  void init();

  void updateTransform();

  ::osg::Group* setupHighlightState(unsigned int state);

 protected:
  /** protected because it's used in LeafNodeCapsule */
  ::osg::GeodeRefPtr landmark_geode_ptr_;

  /** Geode pointer for landmarks */
  ::osg::GeodeRefPtr geode_ptr_;
  /** Alpha value */
  float alpha_;

  /**
   \brief Default constructor
   */
  Node(const std::string& name);

  /** Copy constructor */
  Node(const Node& other);

  /**
   \brief Return the root node to include it in the scene
   */
  ::osg::GroupRefPtr asQueue() const { return transform_ptr_; }

  void setID(const std::string& id_name) {
    id_name_ = id_name;
    switch_node_ptr_->setName(id_name_);
  }

  void setTransparentRenderingBin(bool transparent = true,
                                  osg::StateSet* ss = NULL);

 public:
  static const float TransparencyRenderingBinThreshold;

  /**
   \brief returns rotation and position of the node
   in word frame
   */
  const Configuration& getGlobalTransform() const;

  /**
   \brief getID is a public method for getting the id_name of the Object
   */
  std::string getID() const { return id_name_; }

  bool isDirty() const { return dirty_; }

  void setDirty(bool dirty = true) { dirty_ = dirty; }

  /** Whether this node (and its children) can be selected from mouse.
   */
  bool isSelectable() const {
    return transform_ptr_->getNodeMask() & IntersectionBit;
  }

  /** Set whether this node (and its children) can be selected from mouse.
   */
  void setSelectable(bool selectable = true);

  /** Apply a new global configuration
   */
  inline void applyConfiguration(const osgVector3& position,
                                 const osgQuat& quat) {
    applyConfiguration(Configuration(position, quat));
  }

  /** Apply a new global configuration
   */
  void applyConfiguration(const Configuration& cfg) { M_.set(cfg); }

  /** Set Static transformation
   */
  void setStaticTransform(const osgVector3& position, const osgQuat& quat);

  /** Get Static rotation
   */
  osgQuat getStaticRotation() const;

  /** Get Static position
   */
  osgVector3 getStaticPosition() const;

  /**
   \brief Virtual method for setting the visibility mode of the object : visible
   or not visible \param visibilitymode state
   */
  virtual void setVisibilityMode(const VisibilityMode& visibility_state);

  virtual const VisibilityMode& getVisibilityMode() const {
    return visibilityMode_;
  }

  /**
   \brief Virtual method for setting the lighting mode of the object : influence
   by light or not \param lightingmode state
   */
  virtual void setLightingMode(const LightingMode& lighting_state);

  virtual LightingMode getLightingMode() const;

  /**
   \brief Virtual method for setting the wireframe mode of the object : visible
   or not \param wireframemode state
   */
  virtual void setWireFrameMode(const WireFrameMode& wireframe_state);

  virtual const WireFrameMode& getWireFrameMode() const {
    return selected_wireframe_;
  }

  /** Set scale value of the OSG object */
  void setScale(float scale) { setScale(osgVector3(scale, scale, scale)); }

  /** Set scales value of the OSG object */
  void setScale(const osgVector3& scale) { scale_.set(scale); }

  /** Get scale
   */
  osgVector3 getScale() const { return scale_.value; }

  /** Set the color of the object */
  virtual void setColor(const osgVector4& color) = 0;

  virtual osg::ref_ptr<osg::Node> getOsgNode() const;

  /**
   \brief Return the root node to include it in the scene
   */
  virtual ::osg::GroupRefPtr asGroup() const { return switch_node_ptr_; }

  virtual void addLandmark(const float& size);

  bool hasLandmark() const;

  ::osg::StateSetRefPtr getOrCreateRootStateSet() {
    return switch_node_ptr_->getOrCreateStateSet();
  }

  void deleteLandmark();

  bool getHighlightEnabled() const { return highlight_enabled_; }

  void setHighlightEnabled(bool enabled) {
    setHighlightState(0);
    highlight_enabled_ = enabled;
  }

  void setHighlightState(unsigned int state);

  const std::size_t& getHighlightState() const { return selected_highlight_; }

  /*Tools::ConfigurationPtr_t getConfiguration () const
  {
      Tools::ConfigurationPtr_t configuration =
  Tools::Configuration::create(toDefVector3(auto_transform_ptr_->getPosition()),toEigenQuat(auto_transform_ptr_->getRotation()));
      return configuration;
  }*/

  virtual void setAlpha(const float& alpha);
  virtual float getAlpha() const;

  void setTransparency(const float& transparency);
  float getTransparency() const;

  SCENE_VIEWER_ACCEPT_VISITOR;
  virtual void traverse(NodeVisitor& visitor);

  /* Destructor */
  virtual ~Node();

}; /* class Node */

} /* namespace viewer */
} /* namespace gepetto */

#endif /* dGEPETTO_VIEWER_NODE_HH */
