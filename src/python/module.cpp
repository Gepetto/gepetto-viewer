#include <eigenpy/eigenpy.hpp>
/*#include <eigenpy/eigenpy.hpp>
#include <eigenpy/geometry.hpp>*/
#include "gepetto/python/python.hpp"

#include <iostream>

namespace bp = boost::python;

BOOST_PYTHON_MODULE(libgepetto_pywrap)
{
  /*eigenpy::enableEigenPy();
  eigenpy::exposeAngleAxis();
  eigenpy::exposeQuaternion();

  typedef Eigen::Matrix<double,6,6> Matrix6d;
  typedef Eigen::Matrix<double,6,1> Vector6d;

  eigenpy::enableEigenPySpecific<Matrix6d,Matrix6d>();
  eigenpy::enableEigenPySpecific<Vector6d,Vector6d>();*/

  graphics::python::exposeRobotUpdate();
 graphics::python::exposeRobotViewer();
/*  se3::python::exposeForce();
  se3::python::exposeMotion();
  se3::python::exposeInertia();

  se3::python::exposeModel();
  se3::python::exposeAlgorithms();
  se3::python::exposeParsers();*/
}

