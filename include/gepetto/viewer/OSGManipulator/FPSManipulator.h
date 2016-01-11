#ifndef SCENEVIEWER_FPSMANIPULATOR_H
#define SCENEVIEWER_FPSMANIPULATOR_H

#include <osgGA/FirstPersonManipulator>
#include <osgViewer/Viewer>
#include <osg/Camera>
#include <osgViewer/GraphicsWindow>




namespace osgGA {

  const double startSpeed_ = 2.;
  /** FirstPersonManipulator is base class for camera control based on position
      and orientation of camera, like walk, drive, and flight manipulators. */
  class OSGGA_EXPORT FPSManipulator : public FirstPersonManipulator
  {
          typedef FirstPersonManipulator inherited;

  public:
    FPSManipulator( int flags = DEFAULT_SETTINGS );
    FPSManipulator( const FPSManipulator& fpm, const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY );
    FPSManipulator(osgViewer::Viewer* viewer, int flags = DEFAULT_SETTINGS );

    META_Object( osgGA, FPSManipulator );

  protected :

    virtual bool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
    virtual bool handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
 //   virtual void applyAnimationStep( const double currentProgress, const double prevProgress );
    virtual bool handleFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

    virtual bool handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
    virtual bool handleMouseRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
  //  virtual bool handleMouseWheel( const GUIEventAdapter& ea, GUIActionAdapter& us );

    //virtual bool performMovement(); needed for right clic move

    virtual void rotateRoll( const double roll/*,const osg::Vec3d& localUp */);
    void printHelp();
  private :

    double speed_;
    double speedX_;
    double speedY_;
    double speedZ_;
    double speedRoll_;
    double zNear_;
    double zFar_;
    double fovy_;
    double ratio_;
    osg::Quat rotateRoll_;


    osg::Camera* camera_;
    osgViewer::GraphicsWindow* gWindow_;
    bool rightClic_;
    bool ctrl_;
    bool shift_;


  };// end class


}//namespace osgGA
#endif // FPSMANIPULATOR_H
