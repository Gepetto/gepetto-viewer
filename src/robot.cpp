#include "gepetto/viewer/robot.h"
#include <osgDB/ReadFile>
#include <osg/Switch>
#include <osg/ShapeDrawable>
#include "pinocchio/multibody/model.hpp"

#include <urdf_parser/urdf_parser.h>
using namespace graphics;
Robot::Robot(): osg::Group(),_debugaxes(0)
{
    SE3Model=new se3::Model();
}

Robot::~Robot()
{
    delete SE3Model;
}
Link *Robot::getOsgLink(boost::shared_ptr<urdf::Link >j)
{
    std::map< urdf::Link *,LinkIndex  >::iterator it= _invLinksMap.find(j.get());
    return it!=_invLinksMap.end()? _osglinks[(*it).second ]:0;
}
Joint *Robot::getOsgJoint(boost::shared_ptr<urdf::Joint >j)
{
    std::map< urdf::Joint *,JointIndex  >::iterator it= _invJointsMap.find(j.get());
    return it!=_invJointsMap.end()? _osgjoints[(*it).second ]:0;
}
void  Robot::setUrdfFile(const std::string & s)
{

    _urdfFile=s;
    ///reset robot
    _invJointsMap.clear();
    _invJointsMap.clear();
    _osglinks.clear();
    _osgjoints.clear();
    removeChildren(0,getNumChildren());

    boost::shared_ptr< urdf::ModelInterface > model =
        urdf::parseURDFFile( _urdfFile );
    // Test that file has correctly been parsed
    if (!model)
    {
        throw std::runtime_error (std::string ("Failed to parse ") +
                                  _urdfFile);
    }


    std::vector< boost::shared_ptr < urdf::Link > > links;
    //std::vector< boost::shared_ptr < urdf::Joint > > joints;
    model->getLinks(links);
    //model->getJoints(joints);
    std::string link_name;


    for (unsigned int i = 0 ; i < links.size() ; i++)
    {
        link_name = links[i]->name;
        addLink(links[i]);

    }
}


osg::MatrixTransform * Robot::getGeometryNode( boost::shared_ptr< urdf::Geometry > &geom_shared_ptr)
{
    osg::ref_ptr<osg::Node> node=0;
    osg::Matrix m;
    switch(geom_shared_ptr->type)
    {
    case urdf::Geometry::MESH:
    {
        boost::shared_ptr<  urdf::Mesh > mesh_shared_ptr=boost::static_pointer_cast<  urdf::Mesh >
                ( geom_shared_ptr );

        m.makeScale( mesh_shared_ptr->scale.x,mesh_shared_ptr->scale.x,mesh_shared_ptr->scale.x);
        if (mesh_shared_ptr->filename.substr(0, 10) != "package://")
        {
            throw std::runtime_error ("Error when parsing urdf file: "
                                      "mesh filename should start with"
                                      " \"package://\"");
        }
        std::string  mesh_path =  mesh_shared_ptr->filename.substr
                                  (10, mesh_shared_ptr->filename.size());

        node=  osgDB::readNodeFile(_urdfPackageRootDirectory+mesh_path);//urdf::Geometry
        if(!node)
        {
            std::cerr<<_urdfPackageRootDirectory+mesh_path<<"botfound"<<std::endl;
            return 0;
        }

        break;
    }
    case urdf::Geometry::BOX :
    {
        boost::shared_ptr<  urdf::Box > mesh_shared_ptr=boost::static_pointer_cast<  urdf::Box >( geom_shared_ptr );
        osg::ref_ptr<osg::Geode> geode=new osg::Geode();
        osg::TessellationHints* hints = new osg::TessellationHints;
        hints->setDetailRatio(0.5f);
        //convert urdf to osg
        osg::Vec3 center(0,0,0);
        osg::Vec3 halfLengths(osg::Vec3(mesh_shared_ptr->dim.x,mesh_shared_ptr->dim.y,mesh_shared_ptr->dim.z));
        halfLengths*=0.5f;
        geode->addDrawable(new osg::ShapeDrawable(new osg::Box(center,halfLengths[0],halfLengths[1],halfLengths[2]),hints));
        node=geode;
        break;
    }
    case urdf::Geometry::CYLINDER :
    {
        boost::shared_ptr<  urdf::Cylinder > mesh_shared_ptr=boost::static_pointer_cast<  urdf::Cylinder >( geom_shared_ptr );
        osg::ref_ptr<osg::Geode> geode=new osg::Geode();
        osg::TessellationHints* hints = new osg::TessellationHints;
        hints->setDetailRatio(0.5f);
        //convert urdf to osg
        //double length;  double radius;
//osg::Vec3& center,float radius,float height
        geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0f,0.0f,0.0f),mesh_shared_ptr->radius,mesh_shared_ptr->length),hints));
        node=geode;
        break;
    }
    case urdf::Geometry::SPHERE :
    {
        boost::shared_ptr<  urdf::Sphere > mesh_shared_ptr=boost::static_pointer_cast<  urdf::Sphere >( geom_shared_ptr );
        osg::ref_ptr<osg::Geode> geode=new osg::Geode();
        osg::TessellationHints* hints = new osg::TessellationHints;
        hints->setDetailRatio(0.5f);

        geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),mesh_shared_ptr->radius),hints));
        node=geode;

        break;
    }
    default: //TODO
        break;
    }


    if(node)
    {
        osg::ref_ptr<osg::MatrixTransform > scaler=new osg::MatrixTransform();
        scaler->setDataVariance(osg::Object::STATIC);

        scaler->setMatrix(m);
        scaler->addChild(node);
        return scaler.release();
    }
    return 0;
}
bool Robot::parse (boost::shared_ptr<urdf::Link > &link)
{
    osg::ref_ptr<Link> lastone=0,newone=0;
    lastone=getOsgLink(link  );
    if(!lastone.valid())newone=new Link(link);
    else return true;//don't update previous ...newone=lastone;//update previous version
    osg::ref_ptr<Joint> child=0,parent=0;
    if(link->parent_joint)
    {
        parent=getOsgJoint(link->parent_joint);
        if(!parent.valid())
        {
            /// parent link not parsed yet
            if(!parse(link->parent_joint))        return false;
            parent=getOsgJoint(link->parent_joint);
        }
    }
    else std::cerr<<"link"<< link->name<<" have no joint so it's the root"<<std::endl;



///set State
    newone->setName(link->name);
    if(link->parent_joint)
        parent->addChild(newone);
    ///no parent join == root of skeleton?
    else this->addChild(newone);

    for(  std::vector<boost::shared_ptr<urdf::Joint> >::iterator it=link->child_joints.begin(); it!=link->child_joints.end(); it++)
    {
        child=getOsgJoint(*it);
        if(!child)
        {
            parse(*it);
            child=getOsgJoint(*it);
        }
        newone->addChild(child);

    }

    osg::ref_ptr<osg::Switch> debugswitch= new osg::Switch();

    if(link->visual )
    {

        osg::ref_ptr<osg::MatrixTransform > gr= this->getGeometryNode(link->visual->geometry);
        if(gr)
        {
            newone->setVisualNode(gr);
            newone->addChild(gr);
        }

    }

    if(link->collision )
    {
        osg::ref_ptr<osg::MatrixTransform > gr= this->getGeometryNode(link->collision->geometry);


        if(!_debugaxes )_debugaxes=osgDB::readNodeFile("axes.osgt");
        if(gr)
        {
            gr->addChild(_debugaxes);

            newone->setCollisionNode(gr);
            newone->addChild(gr);

        }

    }



    if(newone->getCollisionNode())

        newone->setValue(newone->getChildIndex(newone->getCollisionNode()),false);


    if(!lastone.valid())
    {
///actual add to model
        _invLinksMap[link.get()]=_links.size();
        _links.push_back(link);
        _osglinks.push_back(newone);
    }
    return true;

}
///SE3Model.addBody (if DOF) + osg::Transform creation
bool Robot::parse (boost::shared_ptr<urdf::Joint > &joint)
{
    osg::ref_ptr<Joint> lastone=0,newone=0;
    lastone=getOsgJoint(joint  );
    if(!lastone.valid())newone=new Joint(joint);
    else return true;//don't update previous ...newone=lastone;//update previous version


///update state

///set index in SE3 or use local joint index (_invLinksMap)
//SE3Model.addBody?

///set osg model
    osg::Matrixf m;
    m.setTrans(osg::Vec3(
                   (float)joint->parent_to_joint_origin_transform.position.x,
                   (float)joint->parent_to_joint_origin_transform.position.y,
                   (float)joint->parent_to_joint_origin_transform.position.z
               ));
    m.setRotate(osg::Quat(
                    (float)joint->parent_to_joint_origin_transform.rotation.x,
                    (float)joint->parent_to_joint_origin_transform.rotation.y,
                    (float)joint->parent_to_joint_origin_transform.rotation.z,
                    (float)joint->parent_to_joint_origin_transform.rotation.w
                ));
    newone->setMatrix(m);

    newone->setName(joint->name);
    if(!lastone.valid())
    {
///actual add to model
        _invJointsMap[joint.get()]=_joints.size();
        _joints.push_back(joint);
        _osgjoints.push_back(newone);
    }
    return true;
}
