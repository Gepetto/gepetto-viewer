// Copyright (c) 2015-2018, LAAS-CNRS
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of gepetto-viewer.
// gepetto-viewer is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// gepetto-viewer is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer. If not, see <http://www.gnu.org/licenses/>.

#include "gepetto/gui/safeapplication.hh"

#include <QDebug>

namespace gepetto {
  namespace gui {
    SafeApplication::SafeApplication(int& argc, char** argv) :
      QApplication(argc, argv)
    {
    }

    bool SafeApplication::notify(QObject *receiver, QEvent *e)
    {
      try {
        return QApplication::notify(receiver, e);
      } catch (const std::exception& e) {
        qDebug () << e.what();
      } catch (...) {
        qDebug() << "Unknown exception";
        qDebug() << "Catch it in SafeApplication::notify";
      }
      return false;
    }
  } // namespace gui
} // namespace gepetto
