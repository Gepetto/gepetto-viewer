//
// Copyright (c) 2020 CNRS
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

#define BOOST_TEST_MODULE properties
#ifndef Q_MOC_RUN
#include <boost/test/unit_test.hpp>
#include <boost/utility/binary.hpp>
#endif

#include <gepetto/viewer/node-property.h>

#include <gepetto/gui/safeapplication.hh>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtConcurrent>
#else
#include <QtCore>
#endif
#include <QApplication>
#include <QCoreApplication>
#include <QDoubleSpinBox>

class PropertyTest : public QObject {
  Q_OBJECT

 public:
  float value;
  bool called;
  PropertyTest() : value(0.), called(false) {}

 public slots:
  void floatChanged(const float& v) {
    value = v;
    called = true;
  }

  void doubleChanged(const double& v) {
    value = (float)v;
    called = true;
  }

 signals:
  void changeFloat(const float& v);
  void changeDouble(const double& v);

 public:
  void setFloat(const float& v) { emit changeFloat(v); }
  void setDouble(const double& v) { emit changeDouble(v); }
};

using namespace gepetto::viewer;

BOOST_AUTO_TEST_SUITE(property)

BOOST_AUTO_TEST_CASE(pfloat1) {
  StoredPropertyTpl<float> property("float");
  property.value = 0.;

  bool setWorked = false;
  bool ok = QMetaObject::invokeMethod(&property, "set", Qt::DirectConnection,
                                      Q_RETURN_ARG(bool, setWorked),
                                      Q_ARG(float, 1.));

  BOOST_CHECK(ok);
  BOOST_CHECK(setWorked);
  BOOST_CHECK_EQUAL(property.value, 1.);
}

BOOST_AUTO_TEST_CASE(pfloat2) {
  StoredPropertyTpl<float>::Ptr_t property(
      new StoredPropertyTpl<float>("float"));
  PropertyTest* ptest = new PropertyTest;
  property->value = 0.;

  ptest->connect(property.get(), SIGNAL(valueChanged(float)),
                 SLOT(floatChanged(float)));

  property->set(1.);
  BOOST_CHECK(ptest->called);
  BOOST_CHECK_EQUAL(property->value, ptest->value);
  delete ptest;
}

BOOST_AUTO_TEST_CASE(pfloat3) {
  int argc = 0;
  char** argv = NULL;
  QCoreApplication application(argc, argv);

  StoredPropertyTpl<float>::Ptr_t property(
      new StoredPropertyTpl<float>("float"));
  PropertyTest* ptestf = new PropertyTest;
  PropertyTest* ptestd = new PropertyTest;
  property->value = 0.;

  ptestf->connect(property.get(), SIGNAL(valueChanged(float)),
                  SLOT(floatChanged(float)), Qt::QueuedConnection);
  ptestd->connect(property.get(), SIGNAL(valueChanged(double)),
                  SLOT(doubleChanged(double)), Qt::QueuedConnection);

  property->set(1.);
  BOOST_CHECK(!ptestf->called);
  BOOST_CHECK(!ptestd->called);
  QCoreApplication::processEvents();
  BOOST_CHECK(ptestf->called);
  BOOST_CHECK(ptestd->called);
}

BOOST_AUTO_TEST_CASE(pfloat4) {
  int argc = 0;
  char** argv = NULL;
  QCoreApplication application(argc, argv);

  StoredPropertyTpl<float>::Ptr_t property(
      new StoredPropertyTpl<float>("float"));
  property->value = 0.;

  PropertyTest* ptestf = new PropertyTest;
  ptestf->connect(property.get(), SIGNAL(valueChanged(float)),
                  SLOT(floatChanged(float)), Qt::QueuedConnection);
  property->connect(ptestf, SIGNAL(changeFloat(float)), SLOT(set(float)),
                    Qt::QueuedConnection);

  ptestf->setFloat(1.);
  BOOST_CHECK(!ptestf->called);
  // PropertyTest -> Property
  QCoreApplication::processEvents();
  BOOST_CHECK(!ptestf->called);
  BOOST_CHECK_EQUAL(property->value, 1.);
  // Property -> PropertyTest
  QCoreApplication::processEvents();
  BOOST_CHECK(ptestf->called);

  PropertyTest* ptestd = new PropertyTest;
  ptestd->connect(property.get(), SIGNAL(valueChanged(double)),
                  SLOT(doubleChanged(double)), Qt::QueuedConnection);
  property->connect(ptestd, SIGNAL(changeDouble(double)), SLOT(set(double)),
                    Qt::QueuedConnection);

  ptestd->setDouble(2.);
  BOOST_CHECK(!ptestd->called);
  // PropertyTest -> Property
  QCoreApplication::processEvents();
  BOOST_CHECK(!ptestd->called);
  BOOST_CHECK_EQUAL(property->value, 2.);
  // Property -> PropertyTest
  QCoreApplication::processEvents();
  BOOST_CHECK(ptestd->called);
}

BOOST_AUTO_TEST_CASE(pfloat5) {
  int argc = 0;
  char** argv = NULL;
  gepetto::gui::SafeApplication application(argc, argv);

  StoredPropertyTpl<float>::Ptr_t property(
      new StoredPropertyTpl<float>("float"));
  property->value = 0.;
  QDoubleSpinBox* dsb = qobject_cast<QDoubleSpinBox*>(property->guiEditor());

  property->set(1.);
  BOOST_CHECK_EQUAL(dsb->value(), 0.);
  BOOST_CHECK_EQUAL(property->value, 1.);
  QCoreApplication::processEvents();
  BOOST_CHECK_EQUAL(dsb->value(), 1.);
  BOOST_CHECK_EQUAL(property->value, 1.);

  dsb->setValue(2.);
  BOOST_CHECK_EQUAL(dsb->value(), 2.);
  BOOST_CHECK_EQUAL(property->value, 1.);
  QCoreApplication::processEvents();
  BOOST_CHECK_EQUAL(property->value, 2.);
  BOOST_CHECK_EQUAL(property->value, 2.);
}

QDoubleSpinBox* pfloat6_multithreaded(QDoubleSpinBox* dsb) {
  dsb->setValue(2.);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
  QThread::sleep(1);
#endif
  return dsb;
}

QDoubleSpinBox* pfloat6_multithreaded1(
    StoredPropertyTpl<float>::Ptr_t property) {
  QDoubleSpinBox* dsb = qobject_cast<QDoubleSpinBox*>(property->guiEditor());
  dsb->setValue(2.);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
  QThread::sleep(1);
#endif
  QCoreApplication::quit();
  return dsb;
}

BOOST_AUTO_TEST_CASE(pfloat6) {
  int argc = 0;
  char** argv = NULL;
  gepetto::gui::SafeApplication application(argc, argv);

  StoredPropertyTpl<float>::Ptr_t property(
      new StoredPropertyTpl<float>("float"));
  property->value = 0.;
  // QDoubleSpinBox* dsb = qobject_cast<QDoubleSpinBox*>(property->guiEditor());

  QFuture<QDoubleSpinBox*> future =
      // QtConcurrent::run(boost::bind(pfloat6_multithreaded, dsb));
      QtConcurrent::run(boost::bind(pfloat6_multithreaded1, property));
  application.exec();

  QDoubleSpinBox* sb = future.result();

  BOOST_REQUIRE(sb);
  BOOST_CHECK_EQUAL(sb->value(), 2.);
  BOOST_CHECK_EQUAL(property->value, 2.);
}

BOOST_AUTO_TEST_SUITE_END()

#include "properties.moc"
