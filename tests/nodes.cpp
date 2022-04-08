//
// Copyright (c) 2019 CNRS
// Authors: Joseph Mirabel
//
//
// This file is part of gepetto-viewer
// gepetto-viewer is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// gepetto-viewer is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer  If not, see
// <http://www.gnu.org/licenses/>.

#define BOOST_TEST_MODULE nodes
#ifndef Q_MOC_RUN
#include <boost/test/unit_test.hpp>
#include <boost/utility/binary.hpp>
#endif

#include <gepetto/viewer/leaf-node-box.h>
#include <gepetto/viewer/node.h>

#define CHECK_VECT_CLOSE(a, b, tol) \
  BOOST_CHECK_SMALL((a - b).length2(), float(tol));

#define CHECK_QUAT_CLOSE(a, b, tol)                     \
  {                                                     \
    osgQuat qres(a.inverse() * b);                      \
    osgVector3 axis;                                    \
    double angle;                                       \
    qres.getRotate(angle, axis);                        \
    if (std::fabs(angle - 2 * M_PI) < std::fabs(angle)) \
      BOOST_CHECK_CLOSE(angle, 2 * M_PI, (double)tol);  \
    else                                                \
      BOOST_CHECK_SMALL(angle, (double)tol);            \
  }

namespace osg {
std::ostream& operator<<(std::ostream& os, const Matrix& m) {
  const char* s = ", ";
  const char* l = "\n";
  os << m(0, 0) << s << m(0, 1) << s << m(0, 2) << s << m(0, 3) << l << m(1, 0)
     << s << m(1, 1) << s << m(1, 2) << s << m(1, 3) << l << m(2, 0) << s
     << m(2, 1) << s << m(2, 2) << s << m(2, 3) << l << m(3, 0) << s << m(3, 1)
     << s << m(3, 2) << s << m(3, 3);
  return os;
}
}  // namespace osg

namespace gepetto {
namespace viewer {
struct NodeTest {
  static void checkAbstractClass(NodePtr_t node) {
    osgQuat q, so;
    osgVector3 t, s;

    // Check transforms
    osgVector3 _ts = node->getStaticPosition();
    osgQuat _qs = node->getStaticRotation();
    osgVector3 ts(1.f, 1.f, 1.f);
    osgQuat qs(1.f, 0.f, 0.f, 0.f);

    node->setDirty(false);
    node->setStaticTransform(ts, qs);
    BOOST_CHECK(node->isDirty());

    CHECK_VECT_CLOSE(node->getStaticPosition(), ts, 1e-4);
    CHECK_QUAT_CLOSE(node->getStaticRotation(), qs, 1e-4);

    node->setDirty(false);
    node->applyConfiguration(osgVector3(0.f, 0.f, 0.f),
                             osgQuat(0.f, 0.f, 0.f, 1.f));
    // Not dirty since configuration did not change.
    BOOST_CHECK(!node->isDirty());
    node->applyConfiguration(osgVector3(0.f, 0.f, 1.f),
                             osgQuat(0.f, 0.f, 0.f, 1.f));
    BOOST_CHECK(node->isDirty());
    node->applyConfiguration(osgVector3(0.f, 0.f, 0.f),
                             osgQuat(0.f, 0.f, 0.f, 1.f));

    const osg::MatrixTransform* matrixTransform =
        dynamic_cast<osg::MatrixTransform*>(node->asQueue().get());
    BOOST_REQUIRE(matrixTransform != NULL);

    matrixTransform->getMatrix().decompose(t, q, s, so);

    CHECK_VECT_CLOSE(t, ts, 1e-4);
    CHECK_QUAT_CLOSE(q, qs, 1e-4);

    osgVector3 t1(0.f, 0.f, 1.f);
    osgQuat q1(0.5f, 0.5f, 0.5f, 0.5f);
    node->applyConfiguration(t1, q1);
    CHECK_VECT_CLOSE(node->getGlobalTransform().position, t1, 1e-4);
    CHECK_QUAT_CLOSE(node->getGlobalTransform().quat, q1, 1e-4);

    matrixTransform->getMatrix().decompose(t, q, s, so);
    // This is very counter intuitive...
    CHECK_VECT_CLOSE(t, (t1 + q1 * ts), 1e-4);
    CHECK_QUAT_CLOSE(q, (qs * q1), 1e-4);

    node->setStaticTransform(_ts, _qs);
  }
};
} /* namespace viewer */
}  // namespace gepetto

using namespace gepetto::viewer;

BOOST_AUTO_TEST_SUITE(node)

BOOST_AUTO_TEST_CASE(box) {
  LeafNodeBoxPtr_t box =
      LeafNodeBox::create("box", osgVector3(0.1f, 0.2f, 0.3f));
  NodeTest::checkAbstractClass(box);
}

BOOST_AUTO_TEST_SUITE_END()
