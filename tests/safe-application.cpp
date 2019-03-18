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

#define BOOST_TEST_MODULE safe_application
#include <boost/test/included/unit_test.hpp>

#include <gepetto/gui/safeapplication.hh>

using namespace gepetto::gui;

class ObjectTest : public QObject
{
  Q_OBJECT

  public:
    bool stdex;

    bool event(QEvent *e)
    {
      (void)e;
      if (stdex) {
        throw std::logic_error ("std logic_error from ObjectTest");
      } else {
        throw 0;
      }
      return true;
    };
};

#include "safe-application.moc"

BOOST_AUTO_TEST_CASE (safeApplication) {
  ObjectTest test;
  QEvent event (QEvent::None);
  int argc = 0;
  SafeApplication app (argc, NULL);
  
  test.stdex = true;
  app.notify (&test, &event);
  test.stdex = false;
  app.notify (&test, &event);
}
