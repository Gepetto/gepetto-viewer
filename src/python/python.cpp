#include "pinocchio/python/python.hpp"
/*#include "pinocchio/python/se3.hpp"
#include "pinocchio/python/force.hpp"
#include "pinocchio/python/motion.hpp"
#include "pinocchio/python/inertia.hpp"

#include "pinocchio/python/model.hpp"
#include "pinocchio/python/data.hpp"
#include "pinocchio/python/algorithms.hpp"*/
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "gepetto/python/graphicrobot.hpp"
#include "gepetto/python/graphicviewer.hpp"

namespace graphics
{
  namespace python
  {
    void exposeRobotUpdate()
    {
      RobotUpdatePythonVisitor::expose();
    //  PyWraperForAlignedStdVector<SE3>::expose("StdVect_RobotUpdate"); //PATE?
    }
    void exposeRobotViewer()
    {
      RobotViewerPythonVisitor<RobotViewer>::expose();
    //  ForcePythonVisitor<Force>::expose();
     // PyWraperForAlignedStdVector<Force>::expose("StdVec_RobotViewer");
    }
  /* void exposeMotion()
    {
      MotionPythonVisitor<Motion>::expose();
      PyWraperForAlignedStdVector<Motion>::expose("StdVec_Motion");
    }
    void exposeInertia()
    {
      InertiaPythonVisitor<Inertia>::expose();
      PyWraperForAlignedStdVector<Inertia>::expose("StdVec_Inertia");
    }
    void exposeModel()
    {
      ModelPythonVisitor::expose();
      DataPythonVisitor::expose();
    }
    void exposeAlgorithms()
    {
      AlgorithmsPythonVisitor::expose();
    }
    void exposeParsers()
    {
      ParsersPythonVisitor::expose();
    }*/
  }} // namespace se3::python
