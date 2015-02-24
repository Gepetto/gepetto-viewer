#ifndef __se3_python_visualrobot_hpp__
#define __se3_python_visualrobot_hpp__

//#include <pinocchio/multibody/model.hpp>
#include <eigenpy/eigenpy.hpp>
#include <eigenpy/geometry.hpp>
#include "pinocchio/python/se3.hpp"
#include <pinocchio/spatial/se3.hpp>
#include <pinocchio/python/se3.hpp>
#include <pinocchio/multibody/joint.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <eigenpy/exception.hpp>
#include <eigenpy/eigenpy.hpp>
#include <numpy/ndarrayobject.h>
#include <gepetto/viewer/robot.h>

#include <gepetto/python/handler.hpp>

namespace graphics
{
namespace python
{
namespace bp = boost::python;

typedef Handler<  RobotUpdate  > RobotUpdateHandler;

struct RobotUpdatePythonVisitor
        : public boost::python::def_visitor< RobotUpdatePythonVisitor >
{
public:
    // typedef Model::Index Index;
    //typedef typename eigenpy::UnalignedEquivalent<Motion>::type Motion_fx;

public:

    /* --- Convert From C++ to Python ------------------------------------- */
    // static PyObject* convert(Model const& modelConstRef)
    // {
    // 	Model * ptr = const_cast<Model*>(&modelConstRef);
    // 	return boost::python::incref(boost::python::object(ModelHandler(ptr)).ptr());
    // }
    static PyObject* convert( RobotUpdateHandler::SmartPtr_t const& ptr)
    {
        return boost::python::incref(boost::python::object(RobotUpdateHandler(ptr)).ptr());
    }

    /* --- Exposing C++ API to python through the handler ----------------- */
    template<class PyClass>
    void visit(PyClass& cl) const
    {
        cl
        .def("setConfiguration",&RobotUpdatePythonVisitor::setConfiguration)
        .def("getVisuJointID",&RobotUpdatePythonVisitor::getJointID)
        .def("setUrdfFile",&RobotUpdatePythonVisitor::setUrdfFile )
        .def("setUrdfPackageDirectory",&RobotUpdatePythonVisitor::setUrdfPackageDirectory)
        .def("setDebugCollisionOnOff",&RobotUpdatePythonVisitor::setDebugCollisionOnOff)
        // .def("createData",&RobotUpdatePythonVisitor::createData)

        .def("__str__",& RobotUpdatePythonVisitor::toString)
        /*
        	  .add_property("nq", &ModelPythonVisitor::nq)
        	  .add_property("nv", &ModelPythonVisitor::nv)
        	  .add_property("nbody", &ModelPythonVisitor::nbody)
        	  .add_property("inertias",
        			bp::make_function(&ModelPythonVisitor::inertias,
        					  bp::return_internal_reference<>())  )
        	  .add_property("jointPlacements",
        			bp::make_function(&ModelPythonVisitor::jointPlacements,
        					  bp::return_internal_reference<>())  )
        	  .add_property("parents",
        			bp::make_function(&ModelPythonVisitor::parents,
        					  bp::return_internal_reference<>())  )
        	  .add_property("names",
        			bp::make_function(&ModelPythonVisitor::names,
        					  bp::return_internal_reference<>())  )
        	  .add_property("gravity",&ModelPythonVisitor::gravity,&ModelPythonVisitor::setGravity)

        	  .def("BuildEmptyModel",&ModelPythonVisitor::maker_empty)
        	  .staticmethod("BuildEmptyModel")
        	  .def("BuildHumanoidSimple",&ModelPythonVisitor::maker_humanoidSimple)
        	  .staticmethod("BuildHumanoidSimple")*/
        ;
    }
///suboptimal (store joint id yourself)
    static int getJointID(  const RobotUpdateHandler & modelPtr,const std::string&name  )
    {
     modelPtr-> beginSyncCall();
        graphics::Robot::Joints::const_iterator it;
        int cpt=0;
        for(it=modelPtr ->getRobot()->getJoints().begin();
                it!=modelPtr ->getRobot()->getJoints().end()&& (*it)->getName()!=name; it++)cpt++;
        if(      it==modelPtr ->getRobot()->getJoints().end()) cpt=-1;
     modelPtr-> endSyncCall();
          return cpt;
    }

    static void setDebugCollisionOnOff (   RobotUpdateHandler & modelPtr  )
    {
        //write model is
        //not thread safe   return modelPtr ->getRobot()->setUrdfFile(name); is wrong
        //so
        modelPtr ->addOrder(
            new SwitchDebugOrder(*modelPtr ->getRobot() ));
    }
    static void setUrdfFile (    RobotUpdateHandler & modelPtr,const std::string&name  )
    {
        //write model is
        //not thread safe   return modelPtr ->getRobot()->setUrdfFile(name); is wrong
        //so
        modelPtr ->addOrder(
            new SetUrdfFileOrder(*modelPtr ->getRobot(),name));
    }

    static void setUrdfPackageDirectory (    RobotUpdateHandler & modelPtr,const std::string&name  )
    {
        //write model is
        //not thread safe   return modelPtr ->getRobot()->setUrdfFile(name); is wrong
        //so
        modelPtr ->addOrder(
            new SetUrdfPackageDirectoryOrder(*modelPtr ->getRobot(),name));
    }
    static void setConfiguration(   RobotUpdateHandler & modelPtr,const int jointid,
                                    const se3::python::SE3PythonVisitor<se3::SE3>::SE3_fx &   M )
    {
        //M.getRotatopn
        //se3::SE3::Quaternion q=(M.rotation);

        if(jointid>=0&&jointid<modelPtr ->getRobot()->getJoints().size())
            modelPtr ->addOrder(   new UpdateJointOrder(*modelPtr ->getRobot()->getJoints()[jointid],M) );
        else std::cerr<<"Warning: Bad Joint Id..you did nothing"<<std::endl;
    }

    /*  static boost::shared_ptr<Data> createData(const ModelHandler& m )
      {	return boost::shared_ptr<Data>( new Data(*m) );      }

      static int nq( ModelHandler & m ) { return m->nq; }
      static int nv( ModelHandler & m ) { return m->nv; }
      static int nbody( ModelHandler & m ) { return m->nbody; }
      static std::vector<Inertia> & inertias( ModelHandler & m ) { return m->inertias; }
      static std::vector<SE3> & jointPlacements( ModelHandler & m ) { return m->jointPlacements; }
      static std::vector<Model::Index> & parents( ModelHandler & m ) { return m->parents; }
      static std::vector<std::string> & names ( ModelHandler & m ) { return m->names; }
      static Motion gravity( ModelHandler & m ) { return m->gravity; }
      static void setGravity( ModelHandler & m,const Motion_fx & g ) { m->gravity = g; }
    */
    static RobotUpdateHandler maker_empty()
    {
        RobotUpdate* ru=new RobotUpdate();
        return RobotUpdateHandler(  ru,true );
    }
    /*static ModelHandler maker_humanoidSimple()
    {
    Model * model = new Model();
    buildModels::humanoidSimple(*model);
    return ModelHandler( model,true );
         }
    */
    static std::string toString(const RobotUpdateHandler& m)
    {
        std::ostringstream s;
        s <<*(unsigned*) (&m. get())<<" "<<m->getRobot()->getName()<<std::endl<<"Joints:"<<std::endl;

        int cpt=0;
        for(graphics::Robot::Joints::const_iterator it=m ->getRobot()->getJoints().begin();
                it!=m ->getRobot()->getJoints().end(); it++)s<<"index "<<cpt++<<"/ name:"<<(*it)->getName()<<std::endl;
        return s.str();
    }

    /* --- Expose --------------------------------------------------------- */
    static void expose()
    {
        bp::class_< std::vector<int> >("StdVec_int")
        .def(bp::vector_indexing_suite< std::vector<int> >());
        bp::class_< std::vector<std::string> >("StdVec_StdString")
        .def(bp::vector_indexing_suite< std::vector<std::string> >());

        bp::class_<RobotUpdateHandler>("RobotUpdate",
                                       "Visual Controller for Articulated rigid body model (const)",
                                       bp::no_init)
        .def(RobotUpdatePythonVisitor());

        /* Not sure if it is a good idea to enable automatic
         * conversion. Prevent it for now */
        //bp::to_python_converter< Model,ModelPythonVisitor >();
        bp::to_python_converter< RobotUpdateHandler::SmartPtr_t,RobotUpdatePythonVisitor >();
    }


};



}
} // namespace se3::python

#endif // ifndef __se3_python_model_hpp__

