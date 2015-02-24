#ifndef OSGROBOT_H
#define OSGROBOT_H

#include <pinocchio/spatial/se3.hpp>
#include <urdf_model/model.h>
#include <osg/MatrixTransform>
#include <osg/Switch>
namespace se3
{
class Model;
}
namespace graphics
{
class Link:public osg::Switch
{
protected:
    boost::shared_ptr<urdf::Link > _urdfmodel;
    osg::ref_ptr<osg::Node> _collisionmesh;
    osg::ref_ptr<osg::Node> _visualmesh;
public:
    Link(): osg::Switch() ,_collisionmesh(0),_visualmesh(0) {}
    Link(boost::shared_ptr<urdf::Link >&l) :osg::Switch()
    {
        _urdfmodel=l;
    }


    Link(const Link&rhs)
    {
        this->_urdfmodel=rhs._urdfmodel;
    }


    Link(const Link&rhs,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY):osg::Switch(rhs,  copyop) {}
    META_Node(graphics, Link);

    ///Properties
    const osg::Node* getVisualNode()const
    {
        return _visualmesh;
    }

    void setVisualNode(  osg::Node*n)
    {
        _visualmesh=n;
    }

    const osg::Node* getCollisionNode()const
    {
        return _collisionmesh;
    }

    void setCollisionNode(  osg::Node*n)
    {
        _collisionmesh=n;
    }
    inline const urdf::Link *getUrdfModel()const
    {
        return _urdfmodel.get() ;
    }
    inline void setUrdfModel(boost::shared_ptr<urdf::Link > &l)
    {
        _urdfmodel=l ;
    }
};


class Joint:public osg::MatrixTransform
{
protected:
    boost::shared_ptr<urdf::Joint > _urdfmodel;
    osg::Matrixf _Mlocal; ///static local transform in parentjoint coord
public:
    Joint() :osg::MatrixTransform() {    }
    Joint(boost::shared_ptr<urdf::Joint >&l) :osg::MatrixTransform()
    {
        setUrdfModel(l);
    }
    Joint(const Joint&rhs)
    {

        setUrdfModel(rhs.getUrdfModel());
    }

    Joint(const Joint&rhs,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY):osg::MatrixTransform(rhs,  copyop) {}
    META_Node(graphics, Joint);

    ///Dangerous User Interface (call it in a updatecallback or in the main osg loop to prevent thread issues)
    void applyConfiguration(const se3::SE3 &conf);

    ///Properties
    inline const boost::shared_ptr<urdf::Joint > getUrdfModel()const
    {
        return _urdfmodel  ;
    }
    inline void setUrdfModel(const boost::shared_ptr<urdf::Joint >&l);
};

class UpdateOrder
{
public:
    virtual void exec()=0;
};



class Robot;
///ThreadSafe Visual Robot Update Callback
///used 4 python scripting
class RobotUpdate: public osg::NodeCallback
{
    OpenThreads::Mutex _mutex; ///async calls for  rw methods
    mutable OpenThreads::Mutex _sync;///sync calls for ro methods
    Robot* _rob;///

    std::list<UpdateOrder*> _stackorders;
public:
    META_Object(graphics,RobotUpdate);
    ///constructors
    RobotUpdate():_rob(0) {};
    RobotUpdate(Robot*r):_rob(r) {}
    ///dummy copyop
    RobotUpdate(const RobotUpdate& nc,const osg::CopyOp&c):osg::NodeCallback(nc,c) {}

    ///get the object for which control interactions are delegated
    Robot * getRobot()const
    {
        return _rob;
    }
///add order
    inline void addOrder(UpdateOrder*o)
    {
        _sync.lock();///prevent ro sync calls
        _mutex.lock();
        _stackorders.push_back(o);

        _mutex.unlock();
    }
    inline void beginSyncCall()const
    {
        _sync.lock();
    }
    inline void endSyncCall()const
    {
        _sync.unlock();
    }

///treat Order Queue
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        ///use addUpdateCallBack on Robot instead of whatUdid...
        assert(nv->getVisitorType()==osg:: NodeVisitor::UPDATE_VISITOR);

        _mutex.lock();
        while(!_stackorders.empty())
        {
            UpdateOrder * order=_stackorders. front();
            order->exec();
            _stackorders.pop_front();
            delete order;
        }

        _mutex.unlock();
        _sync.unlock();///allow ro sync calls
        traverse(node,nv);
    }


};





/**Class representing osg side of a Robot model
*TODO:SE3:Model is attached but not fill yet
*
*/
class Robot:public osg::Group
{

public:

    Robot();

    Robot(const Robot&rhs,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) :osg::Group(rhs,  copyop) {}
    META_Node(graphics, Robot);

    se3::Model * getSE3Model()const
    {
        return SE3Model;
    }
    ///Absolute path to directory containing of urdf package
    void setUrdfPackageRootDirectory(const std::string & s)
    {
        _urdfPackageRootDirectory=s;
    }
    const std::string & getUrdfPackageRootDirectory()const
    {
        return _urdfPackageRootDirectory ;
    }
    ///Absolute path to file containing urdf representation
    void setUrdfFile(const std::string & s);
    const std::string & getUrdfFile()const
    {
        return _urdfFile ;
    }

///add urdf::Link to the model do nothing if already there
///(inner creation of graphics::Link)
    void addLink(boost::shared_ptr<urdf::Link > &link)
    {
        parse(link);
    }
    void removeLink(boost::shared_ptr<urdf::Link > &link)
    {
        ///TODO
    }

    ///add urdf::Joint to the model
    ///(inner creation of graphics::Joint)
    void addJoint(boost::shared_ptr<urdf::Joint > &link)
    {
        parse(link);
    }
    void removeJoint(boost::shared_ptr<urdf::Link > &link)
    {
        ///TODO
    }

    ///get graphics::Joint by urdf::Joint
    Joint *getOsgJoint(boost::shared_ptr<urdf::Joint >j) ;

    ///get graphics::Joint by urdf::Joint
    Link *getOsgLink(boost::shared_ptr<urdf::Link >j);
    typedef std::vector<osg::ref_ptr<Joint > > Joints;
    typedef std::vector<osg::ref_ptr<Link > > Links;



    ///get all graphics::Joints ordored in a vector
    inline const Joints & getJoints()const
    {
        return _osgjoints;
    }
    inline  Joints & getJoints()
    {
        return _osgjoints;
    }
    ///get all graphics::Links ordored in a vector
    inline const Links & getLinks()const
    {
        return _osglinks;
    }
    inline  Links & getLinks()
    {
        return _osglinks;
    }
protected:
    ///Meshes loading
    virtual bool parse (boost::shared_ptr<urdf::Link > &link);
    ///SE3Model.addBody (if DOF) + osg::Transform creation
    virtual bool  parse (boost::shared_ptr<urdf::Joint > &link);
    ~Robot();
    osg::ref_ptr<osg::Node> _debugaxes; ///
    std::string _urdfPackageRootDirectory;
    std::string _urdfFile;
    se3::Model *SE3Model;

protected:
    ///classic async model
    ///typedef std::tuple<urdf::Model *,osg::Model,SE3::Model,...> ModelstoSync
    ///choice done: urdf as entry point and maintain with dirtyflag and use RobotUpdate overload to sync models
    ///store local reference to joint and link for direct memory access
    /// inner indexes
    typedef unsigned int LinkIndex;
    typedef unsigned int JointIndex;
    std::vector<boost::shared_ptr<urdf::Link > > _links;
    std::vector<osg::ref_ptr<Link > > _osglinks;
    std::map< urdf::Link *,LinkIndex  > _invLinksMap;

    std::vector<boost::shared_ptr<urdf::Joint > > _joints;
    std::vector<osg::ref_ptr<Joint > > _osgjoints;
    std::map< urdf::Joint *,JointIndex  > _invJointsMap;
private:
///local helper generate an osg Group from a ::urdf::Mesh
    osg::MatrixTransform * getGeometryNode( boost::shared_ptr<  urdf::Geometry > &);
};



/*********************ThreadSafe Orders for RobotUpdateCallback******************************************************************************/

///Load Urdf Model
class SetUrdfFileOrder:public UpdateOrder
{
public:
    Robot &rob;
    std::string _filename;
    SetUrdfFileOrder(  Robot &r,const std::string &name):rob(r),_filename(name) {}
    virtual void exec()
    {
        rob.setUrdfFile(_filename);
    }

};
///set Urdf Package path
class SetUrdfPackageDirectoryOrder:public UpdateOrder
{
public:
    Robot &rob;
    std::string _filename;
    SetUrdfPackageDirectoryOrder(  Robot &r,const std::string &name):rob(r),_filename(name) {}
    virtual void exec()
    {
        rob.setUrdfPackageRootDirectory(_filename);
    }

};
///Update joint transform
class UpdateJointOrder:public UpdateOrder
{
public:
    Joint &joint;
    se3::SE3 conf;
    osg::Vec3 pos;
    osg::Quat quat;
    UpdateJointOrder(Joint&j,/*const osg::Vec3&v,const osg::Quat&q):joint(j),pos(v),quat(q) {}*/
                     const se3::SE3 & c):joint(j),conf(c) {}
    virtual void exec()
    {
        joint.applyConfiguration(conf);//pos,quat);
    }

};
///switch on/off Collision/Visual Meshes
class SwitchDebugOrder:public UpdateOrder
{
public:
    Robot &rob;
    SwitchDebugOrder(Robot&j):rob(j)  {}
    virtual void exec()
    {
        int index;
        for(Robot::Links::iterator it=rob.getLinks().begin(); it!=rob.getLinks().end(); it++)
        {
            if((*it)->getVisualNode())
            {

                index=  (*it)->getChildIndex((*it)->getVisualNode());
                (*it)->setValue(index,!(*it)->getValue(index));
            }

            if((*it)->getCollisionNode())
            {

                index=  (*it)->getChildIndex((*it)->getCollisionNode());
                (*it)->setValue(index,!(*it)->getValue(index));
            }

        }
    }

};
}
#endif
