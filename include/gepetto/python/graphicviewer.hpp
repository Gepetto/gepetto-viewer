#ifndef __se3_python_visualviewer_hpp__
#define __se3_python_visualviewer_hpp__

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <eigenpy/exception.hpp>
#include <eigenpy/eigenpy.hpp>

#include <gepetto/viewer/viewer.h>

#include <pinocchio/python/motion.hpp>
#include "gepetto/python/graphicrobot.hpp"

#include <OpenThreads/Thread>






namespace graphics
{
  namespace python
  {
    namespace bp = boost::python;

    template<typename RobotViewer>
    struct RobotViewerPythonVisitor
      : public boost::python::def_visitor< RobotViewerPythonVisitor<RobotViewer> >
    {
    /*  typedef typename RobotViewer::Force Force;
      typedef typename RobotViewer::Matrix3 Matrix3;
      typedef typename RobotViewer::Matrix6 Matrix6;
      typedef typename RobotViewer::Vector6 Vector6;
      typedef typename RobotViewer::Vector3 Vector3;

      typedef typename eigenpy::UnalignedEquivalent<RobotViewer>::type RobotViewer_fx;
      typedef typename RobotViewer_fx::Force Force_fx;
      typedef typename RobotViewer_fx::Matrix3 Matrix3_fx;
      typedef typename RobotViewer_fx::Matrix6 Matrix6_fx;
      typedef typename RobotViewer_fx::Vector6 Vector6_fx;
      typedef typename RobotViewer_fx::Vector3 Vector3_fx;*/
typedef  RobotViewer   RobotViewer_fx;
    public:

      static PyObject* convert(RobotViewer const& m)
      {
	RobotViewer_fx m_fx ;//=(RobotViewer*)&m;
	return boost::python::incref(boost::python::object(m_fx).ptr());
      }

      template<class PyClass>
      void visit(PyClass& cl) const
      {
	cl
	/*  .def(bp::init<>(/*<Vector3_fx,Vector3_fx>
	       ((bp::arg("linear"),bp::arg("angular")

		"Initialize the viewer"))*/

		//.def("RobotViewer",*maker_empty())
		.def("addNewRobot",&RobotViewerPythonVisitor::addNewRobot)
		.def("start",&RobotViewerPythonVisitor::start)
		.def("stop",&RobotViewerPythonVisitor::stop)
	 // .def("createData",&RobotViewerPythonVisitor::createData)

	  .def("__str__",& RobotViewerPythonVisitor::toString)
	 /* .def(bp::init<Vector6_fx>((bp::arg("Vector 6d")),"Init from vector 6 [f,n]"))

	  .add_property("linear",&RobotViewerPythonVisitor::getLinear,&RobotViewerPythonVisitor::setLinear)
	  .add_property("angular",&RobotViewerPythonVisitor::getAngular,&RobotViewerPythonVisitor::setAngular)
	  .def("vector",&RobotViewer_fx::toVector)
	  .def("se3Action",&RobotViewer_fx::se3Action)
	  .def("se3ActionInverse",&RobotViewer_fx::se3ActionInverse)

	  .def("cross_motion",&RobotViewerPythonVisitor::cross_motion)
	  .def("cross_force",&RobotViewerPythonVisitor::cross_force)

	  .def("__str__",&RobotViewerPythonVisitor::toString)
	  .add_property("np",&RobotViewer_fx::toVector)

	  .def("Random",&RobotViewer_fx::Random)
	  .staticmethod("Random")
	  .def("Zero",&RobotViewer_fx::Zero)
	  .staticmethod("Zero")*/
	  ;
	  }

  /*    static Vector3_fx getLinear( const RobotViewer_fx & self ) { return self.linear(); }
      static void setLinear( RobotViewer_fx & self, const Vector3_fx & R ) { self.linear(R); }
      static Vector3_fx getAngular( const RobotViewer_fx & self ) { return self.angular(); }
      static void setAngular( RobotViewer_fx & self, const Vector3_fx & R ) { self.angular(R); }

      static RobotViewer_fx cross_motion( const RobotViewer_fx& m1,const RobotViewer_fx& m2 ) { return m1.cross(m2); }
      static Force_fx cross_force( const RobotViewer_fx& m,const Force_fx& f ) { return m.cross(f); }

      static std::string toString(const RobotViewer_fx& m)
      {	  std::ostringstream s; s << m; return s.str();       }

      static void expose()
      {
	bp::class_<RobotViewer_fx>("RobotViewer",
			     "RobotViewer vectors, in se3* == F^6.\n\n"
			     "Supported operations ...",
			     bp::init<>())
	  .def(RobotViewerPythonVisitor<RobotViewer>())
	;

	bp::to_python_converter< RobotViewer,RobotViewerPythonVisitor<RobotViewer> >();
    }*/
  static RobotUpdateHandler addNewRobot(   RobotViewer_fx & modelPtr )
      { RobotUpdate* ru=modelPtr .createRobot();
      return  RobotUpdateHandler(ru,true);
}
  static void start(   RobotViewer_fx & modelPtr )
      { modelPtr.start();
}

  static void stop(   RobotViewer_fx & modelPtr )
      { modelPtr.stop();
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
      static RobotViewer gravity( ModelHandler & m ) { return m->gravity; }
      static void setGravity( ModelHandler & m,const RobotViewer_fx & g ) { m->gravity = g; }
*/
static osg::ref_ptr<RobotViewer>  singleton;
      static RobotViewer_fx* maker_empty()
      {
      if(!singleton.valid()){
      OpenThreads::Thread::Init();
      singleton=new RobotViewer();
        std::cerr<<"realized"<<std::endl;
      }
        std::cerr<<"realized"<<std::endl;
      singleton->start();
	return  singleton.get();//RobotViewerHandler( new RobotViewer(),true );
      }
      /*static ModelHandler maker_humanoidSimple()
      {
	Model * model = new Model();
	buildModels::humanoidSimple(*model);
	return ModelHandler( model,true );
      }
*/
      static std::string toString(const RobotViewer_fx& m)
      {	  std::ostringstream s; s <<*(unsigned*)  (&m  ); return s.str();       }

      /* --- Expose --------------------------------------------------------- */
      static void expose()
      {  OpenThreads::Thread::Init();
        std::cerr<<"OpenThreads::Thread::Init"<<std::endl;
	bp::class_< std::vector<int> >("StdVec_int")
	  .def(bp::vector_indexing_suite< std::vector<int> >());
	bp::class_< std::vector<std::string> >("StdVec_StdString")
	  .def(bp::vector_indexing_suite< std::vector<std::string> >());
bp::class_<RobotViewer_fx, boost::noncopyable>("RobotViewer")
/*	bp::class_<RobotViewer_fx>("RobotViewer",
				 "osg::CompositeViewer wrapped for specialized usage (const)" ,
				    bp::init<>()) //bp::no_init)*/
	  .def(RobotViewerPythonVisitor())
	 // .def(maker_empty())
	  ;

	/* Not sure if it is a good idea to enable automatic
	 * conversion. Prevent it for now */
	//bp::to_python_converter< Model,ModelPythonVisitor >();
	bp::to_python_converter< RobotViewer ,RobotViewerPythonVisitor <RobotViewer > >();
      }


    };



  }} // namespace se3::python


#endif // ifndef __se3_python_model_hpp__
