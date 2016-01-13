//
//  KeyboardManipulator
//  gepetto-viewer
//
//  Alternative CameraManipulator for OSG, use keyboard and mouse
//  KeyBinding are inspired by the classic system in games
//
//  Created by Pierre Fernbach in january 2016
//
#include <stdlib.h>
#include <stdio.h>
#include <gepetto/viewer/OSGManipulator/keyboard-manipulator.h>
#include <gepetto/viewer/config-osg.h>



/**
 * CameraManipulator which use keyboard for moving the camera, same deplacement as most of the FPS games :
 *
 * independant of keyboard layout (use the active keyboard layout when the osg::viewer constructor is called) , only use physical position of the key
 * keybinding for azerty (qwerty) users :
 * z/s (w/s) : move forward/backward
 * q/d (a/d) : move left/right
 * a/e (a/e) : rotation (roll) left/right
 * space/c : move up/down
 * rotation yaw and pitch with the mouse (keep left button pressed)
 *
 */
using namespace osg;
using namespace osgGA;

/// Constructor.
KeyboardManipulator::KeyboardManipulator( int flags )
   : inherited( flags )
{
  speed_=startSpeed_;
  speedRoll_=0.;
  speedX_=0.;
  speedY_=0.;
  speedZ_=0.;
  ctrl_ = false;
  shift_ = false;
  rightClic_ = false;
  keyLayout_=LAYOUT_unknown;
  localUp_ = getUpVector( getCoordinateFrame( _eye ) );
  noRoll_=true;
  setAllowThrow(false);// stop all mouse motions when mouse is released
  //display_=XOpenDisplay(0);
  initKeyboard();
}

/// Constructor with reference to the graphic window, needed for hidding mouse cursor
KeyboardManipulator::KeyboardManipulator(osgViewer::GraphicsWindow *window, int flags)
  : inherited( flags ),gWindow_(window)/*,camera_(viewer->getCamera())*/
{
  speed_=startSpeed_;
  speedRoll_=0.;
  speedX_=0.;
  speedY_=0.;
  speedZ_=0.;
  ctrl_ = false;
  shift_ = false;
  rightClic_ = false;
  keyLayout_=LAYOUT_unknown;
  localUp_ = getUpVector( getCoordinateFrame( _eye ) );
  noRoll_=true;
  setAllowThrow(false);// stop all mouse motions when mouse is released
 /* osgViewer::Viewer::Windows windows;
  window->getWindows(windows);
  gWindow_=windows.front();*/

  initKeyboard();
}


/// Copy Constructor.
KeyboardManipulator::KeyboardManipulator( const KeyboardManipulator& fpm, const CopyOp& copyOp )
   :osg::Object(fpm, copyOp), osg::Callback(fpm, copyOp),inherited( fpm, copyOp )
{
  initKeyboard();
}


// pressing a key
bool KeyboardManipulator::handleKeyDown( const GUIEventAdapter& ea, GUIActionAdapter& us )
{

  //keycode_ = XKeysymToKeycode(display_,ea.getUnmodifiedKey());

  int keySym = ea.getUnmodifiedKey();
  if(keyLayout_ == LAYOUT_qwerty){ // adapt to qwerty keyboard
      switch(keySym){
        case  osgGA::GUIEventAdapter::KEY_W :
          keySym = osgGA::key_forward;
        break;
        case  osgGA::GUIEventAdapter::KEY_Q :
          keySym = osgGA::key_roll_left;
        break;
        case  osgGA::GUIEventAdapter::KEY_A :
          keySym = osgGA::key_left;
        break;
      }
  }

  switch(keySym)
  {
    case osgGA::key_forward :
    case osgGA::GUIEventAdapter::KEY_Up :
      // move forward
      if(speedX_ <= 0){
        speedX_ =1.;
        return true;
        }
      else
         return false;
    break;
    case osgGA::key_backward :
    case osgGA::GUIEventAdapter::KEY_Down :
      // move backward
      if(speedX_ >=0){
        speedX_ =-1.;
        return true;
        }
      else
        return false;
    break;
    case osgGA::key_left :
    case osgGA::GUIEventAdapter::KEY_Left :
      // move left
      if(speedY_ >= 0){
        speedY_ = -1.;
        return true;
        }
      else
        return false;
    break;
    case osgGA::key_right :
    case osgGA::GUIEventAdapter::KEY_Right :
      // move right
      if(speedY_ <= 0){
        speedY_ = 1.;
        return true;
        }
      else
        return false;
    break;
    case osgGA::key_up : //spacebar
      // move up
      if(speedZ_ <= 0){
        speedZ_ = 1.;
        return true;
        }
      else
        return false;
    break;
    case osgGA::key_down :
      // move down
      if(speedZ_ >= 0 ){
        speedZ_ = -1.;
        return true;
        }
      else
        return false;
    break;
    case osgGA::key_roll_left :
      // roll rotation left
      noRoll_=false;
      if (speedRoll_ >=0){
        speedRoll_ = -1.;
        return true;
        }
      else
        return false;
    break;
    case osgGA::key_roll_right :
      // roll rotation right
      noRoll_=false;
      if(speedRoll_ <=0){
        speedRoll_ = 1.;
        return true;
        }
      else
        return false;
    break;
    case osgGA::GUIEventAdapter::KEY_R:
      flushMouseEventStack();
      _thrown = false;
      localUp_ = getUpVector( getCoordinateFrame( _eye ) );
      home(ea,us);
      return true;
    break;
    case osgGA::GUIEventAdapter::KEY_H :
    case osgGA::GUIEventAdapter::KEY_Comma :
    case osgGA::GUIEventAdapter::KEY_Question :
      getUsage();
      return false;
    break;
    case osgGA::GUIEventAdapter::KEY_Plus :
    case 65451 :// '+'  numpad
      speed_*=1.2;
      return false;
    break;
    case osgGA::GUIEventAdapter::KEY_Minus :
    case 65453 : // '-'  numpad
      speed_*=0.8;
      return false;
    break;
    case osgGA::GUIEventAdapter::KEY_Asterisk :
    case 65450 : // '*'  numpad
      speed_=startSpeed_; // reset speed
      return false;
    break;
    case osgGA::GUIEventAdapter::KEY_Control_L:
    case osgGA::GUIEventAdapter::KEY_Control_R:
        ctrl_ = true;
    break;
    case osgGA::GUIEventAdapter::KEY_Shift_L:
    case osgGA::GUIEventAdapter::KEY_Shift_R:
        shift_ = true;
    break;
  }


  return false;
}


/// Releasing the key
bool KeyboardManipulator::handleKeyUp( const GUIEventAdapter& ea, GUIActionAdapter& /*us*/ )
{
  //std::cout<<"key : "<<ea.getKey()<<" unmodified code : "<<ea.getUnmodifiedKey()<<" keyMask : "<<ea.getModKeyMask()<<std::endl;


 // keycode_ = XKeysymToKeycode(display_,ea.getUnmodifiedKey());
 // std::cout<<"keycode = "<<keycode_<<std::endl;

  int keySym = ea.getUnmodifiedKey();
  if(keyLayout_ == LAYOUT_qwerty) { // adapt to qwerty keyboard
      switch(keySym){
        case  osgGA::GUIEventAdapter::KEY_W :
          keySym = osgGA::key_forward;
        break;
        case  osgGA::GUIEventAdapter::KEY_Q :
          keySym = osgGA::key_roll_left;
        break;
        case  osgGA::GUIEventAdapter::KEY_A :
          keySym = osgGA::key_left;
        break;
      }
  }

  switch(keySym){
    case osgGA::key_forward :
    case osgGA::key_backward :
    case osgGA::GUIEventAdapter::KEY_Up :
    case osgGA::GUIEventAdapter::KEY_Down :
      speedX_ =0.;
      return false;
    break;
    case osgGA::key_right :
    case osgGA::key_left :
    case osgGA::GUIEventAdapter::KEY_Right :
    case osgGA::GUIEventAdapter::KEY_Left :
      speedY_ = 0.;
      return false;
    break;
    case osgGA::key_up : //spacebar
    case osgGA::key_down :
      speedZ_=0.;
      return false;
    break;
    case osgGA::key_roll_left :
    case osgGA::key_roll_right :
      speedRoll_=0.;
      return false;
    break;
    }
  switch(ea.getKey()){
    case '2' :
      getUsage();
    break;
    case osgGA::GUIEventAdapter::KEY_Control_L:
    case osgGA::GUIEventAdapter::KEY_Control_R:
        ctrl_ = false;
    break;
    case osgGA::GUIEventAdapter::KEY_Shift_L:
    case osgGA::GUIEventAdapter::KEY_Shift_R:
        shift_ = false;
    break;
   }
  return false;
}


void KeyboardManipulator::rotateRoll(const double roll/*,const osg::Vec3d& localUp */)
{
    //bool verticalAxisFixed = (localUp != Vec3d( 0.,0.,0. ));

    // fix current rotation

    // rotations
    rotateRoll_.makeRotate( roll,_rotation * Vec3d( 0.,0.,-1. ) );
    _rotation = _rotation * rotateRoll_;
}

// free rotation (remove localUp constraint from parent class)
bool KeyboardManipulator::performMovementLeftMouseButton( const double /*eventTimeDelta*/, const double dx, const double dy )
{
  rotateYawPitch( _rotation, dx, dy, localUp_ );

  return true;
}


// called at each refresh, need to check the speed and move camera accordingly
bool KeyboardManipulator::handleFrame( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
  double current_frame_time = ea.getTime();

  _delta_frame_time = current_frame_time - _last_frame_time;
  _last_frame_time = current_frame_time;

  double dist = speed_ * _delta_frame_time;  // distance according to refresh rate

  if(ctrl_)
    dist = dist / 10. ;

  rotateRoll_.makeRotate(dist*speedRoll_, _rotation * Vec3d( 0.,0.,-1. ) );  // apply rotation
  _rotation = _rotation * rotateRoll_;

  _eye += _rotation * Vec3d (dist*speedY_, dist * speedZ_, - dist * speedX_);


  return inherited::handleFrame(ea,us);
}

// method overrided for hidding the mouse cursor when the view move :
bool KeyboardManipulator::handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us ){
  gWindow_->useCursor(false);
  if( ! noRoll_)
    localUp_ = _rotation * Vec3d( 0.,1.,0. );

  return inherited::handleMousePush(ea,us);
}

bool KeyboardManipulator::handleMouseRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us ){
  gWindow_->useCursor(true);
  noRoll_ = true;
  return inherited::handleMouseRelease(ea,us);
}

/*bool KeyboardManipulator::handleMouseWheel( const GUIEventAdapter& ea, GUIActionAdapter& us ){
  osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();

  // handle centering
  if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
  {
      if( ((sm == GUIEventAdapter::SCROLL_DOWN) && (_wheelMovement > 0.)) ||
          ((sm == GUIEventAdapter::SCROLL_UP)   && (_wheelMovement < 0.)) )
      {
          // stop thrown animation
          _thrown = false;
          if( getAnimationTime() <= 0. )
              // center by mouse intersection (no animation)
              setCenterByMousePointerIntersection( ea, us );
          else {
              // start new animation only if there is no animation in progress
              if( !isAnimating() )
                  startAnimationByMousePointerIntersection( ea, us );
          }
      }
  }

  switch( sm )
  {
      // mouse scroll up event
      case GUIEventAdapter::SCROLL_UP: // increase clipping
      {
        camera_->getProjectionMatrixAsPerspective(fovy_,ratio_,zNear_,zFar_);  // initialise value with current setting
        if(ctrl_){// increase far distance view
            zFar_ +=1;
          }
        else{ // reduce close distance view
            zNear_ -= 1;
            if (zNear_<0.1)
              zNear_=0.1;
          }
        std::cout<<"zNear = "<<zNear_<<"  zFar = "<<zFar_<<std::endl;
        camera_->setProjectionMatrixAsPerspective(fovy_,ratio_,zNear_,zFar_);
        return false;
      }
      // mouse scroll down event
      case GUIEventAdapter::SCROLL_DOWN:  // reduce clipping
      {
        camera_->getProjectionMatrixAsPerspective(fovy_,ratio_,zNear_,zFar_);  // initialise value with current setting
        if(ctrl_){// reduce far distance view
            zFar_ -=1;
            if(zFar_ <= zNear_)
              zFar_ = zNear_ +0.1;
          }
        else{ // increase close distance view
            zNear_ += 1;
          }
        std::cout<<"zNear = "<<zNear_<<"  zFar = "<<zFar_<<std::endl;
        camera_->setProjectionMatrixAsPerspective(fovy_,ratio_,zNear_,zFar_);
        return false;
      }
      // unhandled mouse scrolling motion
      default:
          return false;
  }
}*/


// if rightClic is activated, do the same as performLeftMouvement
/*bool KeyboardManipulator::performMovement(){
  if(rightClic_){
    // return if less then two events have been added
    if( _ga_t0.get() == NULL || _ga_t1.get() == NULL )
        return false;

    // get delta time
    double eventTimeDelta = _ga_t0->getTime() - _ga_t1->getTime();
    if( eventTimeDelta < 0. )
    {
        OSG_WARN << "Manipulator warning: eventTimeDelta = " << eventTimeDelta << std::endl;
        eventTimeDelta = 0.;
    }

    // get deltaX and deltaY
    float dx = _ga_t0->getXnormalized() - _ga_t1->getXnormalized();
    float dy = _ga_t0->getYnormalized() - _ga_t1->getYnormalized();

    // return if there is no movement.
    if( dx == 0. && dy == 0. )
        return false;

    performMovementLeftMouseButton(eventTimeDelta,dx,dy);
  }

  return inherited::performMovement();
}*/

// check if the current keyboard is azerty or qwerty and adapt the keybinding
bool KeyboardManipulator::initKeyboard(){

  char buf[128];
  FILE *fp;
  // send system command and get the output
  if ((fp = popen("LANG=C LC_ALL=C setxkbmap -print", "r")) == NULL) {
     std::cout<<"Error sending terminal command !"<<std::endl;
     return false;
  }

  fgets(buf, 128, fp);
  fgets(buf, 128, fp);  // the second line of the output contain either "azerty" or "qwerty"
  std::string output(buf);

  if(output.find("azerty") != std::string::npos){
    keyLayout_=LAYOUT_azerty;
    //std::cout<<"azerty keyboard detected"<<std::endl;
  }
  else if(output.find("qwerty") != std::string::npos){
    keyLayout_=LAYOUT_qwerty;
    //std::cout<<"qwerty keyboard detected"<<std::endl;
  }
  else
    std::cout<<"Unknow keyboard layout"<<std::endl;


  if(pclose(fp))  {
     std::cout<<"Command not found or exited with error status"<<std::endl;
     return false;
  }

  return true;
}


void KeyboardManipulator::getUsage(){
  std::cout<<"###################################################"<<std::endl;
  std::cout<<"#              Usage of FPSManipulator :          #"<<std::endl;
  std::cout<<"# Translation with the keyboard                   #"<<std::endl;
  if(keyLayout_ != LAYOUT_qwerty){
    std::cout<<"# Planar translation with arrow key or zqsd       #"<<std::endl;
    std::cout<<"# z/s  : forward/backward                         #"<<std::endl;
    std::cout<<"# q/d  : left/right                               #"<<std::endl;
    std::cout<<"# a/e  : roll rotation                            #"<<std::endl;
  }  else {
    std::cout<<"# Planar translation with arrow key or wasd       #"<<std::endl;
    std::cout<<"# w/s : forward/backward                          #"<<std::endl;
    std::cout<<"# a/d : left/right                                #"<<std::endl;
    std::cout<<"# q/e : roll rotation                             #"<<std::endl;
  }
  std::cout<<"# space/c : up/down                               #"<<std::endl;
  std::cout<<"# Mouse (left button) : yaw/pitch rotation        #"<<std::endl;
  std::cout<<"#            -------------------------            #"<<std::endl;
  std::cout<<"# r : Reset the view                              #"<<std::endl;
  std::cout<<"# hold Ctrl : Slow mouvements                     #"<<std::endl;
  std::cout<<"# + / - : change mouvement speed (keyboard only)  #"<<std::endl;
  std::cout<<"# * : reset mouvement speed (keyboard only)       #"<<std::endl;
  std::cout<<"# h : display this message                        #"<<std::endl;
  std::cout<<"# 1 : switch back to trackball manipulator        #"<<std::endl;
  std::cout<<"###################################################"<<std::endl;



}
