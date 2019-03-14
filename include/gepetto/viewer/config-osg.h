//
//  config-osg.h
//  gepetto-viewer
//
//  Created by Justin Carpentier on 06/02/2014.
//  Modified by Mathieu Geisert in November 2014.
//  Copyright (c) 2014 LAAS-CNRS. All rights reserved.
//

#ifndef DynAndGraph_config_h
#define DynAndGraph_config_h

#include <QMetaType>

#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Geometry>

#include <osg/Geode>
#include <osg/PositionAttitudeTransform>
#include <osg/AutoTransform>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Group>
#include <osg/Switch>
#include <osg/ClearNode>
#include <osg/LineWidth>

#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/StateSet>
#include <osg/Depth>

#include <osg/State>
#include <osg/StateSet>
#include <osg/Light>
#include <osg/LightSource>

#include <osg/Material>

#include <osg/Camera>
#include <osg/Viewport>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerBase>
#include <osg/GraphicsContext>

#include <osg/PolygonMode>

#include <gepetto/viewer/fwd.h>
#include <gepetto/viewer/macros.h>

namespace osg
{
    
    DEF_CLASS_SMART_PTR(Geode)
    
    DEF_OSG_CLASS_REF_PTR(Sphere)
    DEF_OSG_CLASS_REF_PTR(Box)
    DEF_OSG_CLASS_REF_PTR(Cone)
    DEF_OSG_CLASS_REF_PTR(Cylinder)
    DEF_OSG_CLASS_REF_PTR(Capsule)
    DEF_OSG_CLASS_REF_PTR(ShapeDrawable)
    DEF_OSG_CLASS_REF_PTR(Geometry)
    DEF_OSG_CLASS_REF_PTR(LineWidth)
    
    DEF_OSG_CLASS_REF_PTR(Geode)
    DEF_OSG_CLASS_REF_PTR(PositionAttitudeTransform)
    DEF_OSG_CLASS_REF_PTR(AutoTransform)
    DEF_OSG_CLASS_REF_PTR(MatrixTransform)
    DEF_OSG_CLASS_REF_PTR(Light)
    DEF_OSG_CLASS_REF_PTR(LightSource)

    DEF_OSG_CLASS_REF_PTR(Node)
    DEF_OSG_CLASS_REF_PTR(Group)
    DEF_OSG_CLASS_REF_PTR(Switch)
    DEF_OSG_CLASS_REF_PTR(ClearNode)
    
    DEF_OSG_CLASS_REF_PTR(Depth)
    
    DEF_OSG_CLASS_REF_PTR(AlphaFunc)
    DEF_OSG_CLASS_REF_PTR(BlendFunc)
    DEF_OSG_CLASS_REF_PTR(StateSet)
    
    DEF_OSG_CLASS_REF_PTR(State)
    DEF_OSG_CLASS_REF_PTR(StateSet)
    
    DEF_OSG_CLASS_REF_PTR(Material)
    
    DEF_OSG_CLASS_REF_PTR(Camera)
    DEF_OSG_CLASS_REF_PTR(Viewport)
    DEF_OSG_CLASS_REF_PTR(GraphicsContext)
    
    DEF_OSG_CLASS_REF_PTR(Vec3Array)
    DEF_OSG_CLASS_REF_PTR(Vec4Array)
  
    DEF_OSG_CLASS_REF_PTR(PolygonMode)
        
    typedef ::osg::ref_ptr< GraphicsContext::Traits > TraitsRefPtr;
  
} /* namespace osg */

namespace osgViewer
{
    DEF_OSG_CLASS_REF_PTR(Viewer)
    
} /* namespace osgViewer */

typedef ::osg::Vec2f osgVector2;
typedef ::osg::Vec3f osgVector3;
typedef ::osg::Vec4f osgVector4;

typedef ::osg::Quat osgQuat;

namespace gepetto {
namespace viewer {
    
#define DEF_WIDTH_WINDOW 600
#define DEF_HEIGHT_WINDOW 480
 
    enum VisibilityMode
    {
        VISIBILITY_ON = 0,
        ALWAYS_ON_TOP,
        VISIBILITY_OFF
    };
    
    enum LightingMode
    {
        LIGHT_INFLUENCE_ON = 0,
        LIGHT_INFLUENCE_OFF
    };
    
    enum WireFrameMode
    {
        FILL = 0,
        WIREFRAME,
        FILL_AND_WIREFRAME
    };
    
    enum FaceMode
    {
        FACE = 0,
        EDGE,
        FACE_AND_EDGE
    };     

    struct Configuration
    {
        osgVector3 position;
        osgQuat quat;
        Configuration() {}
        /// \param XYZW when false, the 4 last parameters are a quaternion (w,x,y,z)
        ///             otherwise, a quaternion (x,y,z,w)
        explicit Configuration(const float* a, bool XYZW)
          : position(a[0],a[1],a[2])
          , quat(a[(XYZW ? 3 : 4)],
                 a[(XYZW ? 4 : 5)],
                 a[(XYZW ? 5 : 6)],
                 a[(XYZW ? 6 : 3)]) {}
        Configuration(const osgVector3& p, const osgQuat& q) : position(p), quat(q) {}
    };
    
} /* namespace viewer */
} /* namespace gepetto */

Q_DECLARE_METATYPE(gepetto::viewer::Configuration)

#endif
