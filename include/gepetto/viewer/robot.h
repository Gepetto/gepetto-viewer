#ifndef OSGROBOT_H
#define OSGROBOT_H


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
public:
    Joint() :osg::MatrixTransform() {    }
    Joint(boost::shared_ptr<urdf::Joint >&l) :osg::MatrixTransform()
    {
        _urdfmodel=l;
    }
    Joint(const Joint&rhs)
    {
        this->_urdfmodel=rhs._urdfmodel;
    }

    Joint(const Joint&rhs,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY):osg::MatrixTransform(rhs,  copyop) {}
    META_Node(graphics, Joint);

    ///Dangerous User Interface (call it in a updatecallback or in the main osg loop to prevent thread issues)
    inline void applyConfiguration(const osg::Vec3&v,const osg::Quat&q)
    {
        osg::Matrixd m;
        m.setTrans(v);
        m.setRotate(q);
        setMatrix(m);
    }

    ///Properties
    inline const urdf::Joint *getUrdfModel()const
    {
        return _urdfmodel.get() ;
    }
    inline void setUrdfModel(boost::shared_ptr<urdf::Joint >&l)
    {
        _urdfmodel=l ;
    }
};

class UpdateOrder
{
public:
    virtual void exec()=0;
};



class Robot;
///ThreadSafe Visual Robot Update Callback
class RobotUpdate: public osg::NodeCallback
{
    OpenThreads::Mutex _mutex;
    Robot* _rob;

    std::list<UpdateOrder*> _stackorders;
public:
    META_Object(graphics,RobotUpdate);
    RobotUpdate():_rob(0) {};
    RobotUpdate(Robot*r):_rob(r) {}
    RobotUpdate(const RobotUpdate& nc,const osg::CopyOp&c):osg::NodeCallback(nc,c) {}

    Robot * getRobot()const{return _rob;}
///add order
    inline void addOrder(UpdateOrder*o)
    {
        _mutex.lock();
        _stackorders.push_back(o);
        _mutex.unlock();
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
        traverse(node,nv);
    }


};
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

    void setUrdfPackageRootDirectory(const std::string & s)
    {
        _urdfPackageRootDirectory=s;
    }
    const std::string & getUrdfPackageRootDirectory()const
    {
        return _urdfPackageRootDirectory ;
    }
    void setUrdfFile(const std::string & s);
    const std::string & getUrdfFile()const
    {
        return _urdfFile ;
    }

    void addLink(boost::shared_ptr<urdf::Link > &link)
    {
        parse(link);
    }
    void addJoint(boost::shared_ptr<urdf::Joint > &link)
    {
        parse(link);
    }
    Joint *getOsgJoint(boost::shared_ptr<urdf::Joint >j) ;
    Link *getOsgLink(boost::shared_ptr<urdf::Link >j);
    typedef std::vector<osg::ref_ptr<Joint > > Joints;
    typedef std::vector<osg::ref_ptr<Link > > Links;
    inline Joints & getJoints()
    {
        return _osgjoints;
    }
    inline Links & getLinks()
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

///ThreadSafe Order for RobotUpdateCallback
///Update joint transform
class UpdateJointOrder:public UpdateOrder
{
public:
    Joint &joint;
    osg::Vec3 pos;
    osg::Quat quat;
    UpdateJointOrder(Joint&j,const osg::Vec3&v,const osg::Quat&q):joint(j),pos(v),quat(q) {}
    virtual void exec()
    {
        joint.applyConfiguration(pos,quat);
    }

};
///switch on/off Collision/Visual Meshes
class SwitchDebugOrder:public UpdateOrder
{
public:
    Robot &rob;
    SwitchDebugOrder(Robot&j):rob(j)  {}
    virtual void exec()
    { int index;
        for(Robot::Links::iterator it=rob.getLinks().begin(); it!=rob.getLinks().end(); it++)
        {
        if((*it)->getVisualNode()){

     index=  (*it)->getChildIndex((*it)->getVisualNode());
      (*it)->setValue(index,!(*it)->getValue(index));
        }

        if((*it)->getCollisionNode()){

      index=  (*it)->getChildIndex((*it)->getCollisionNode());
      (*it)->setValue(index,!(*it)->getValue(index));
        }

    }
    }

};
}
#endif
