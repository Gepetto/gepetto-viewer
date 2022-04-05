// Copyright (c) 2020, Joseph Mirabel
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer. If not, see <http://www.gnu.org/licenses/>.

#include <QString>
#include <gepetto/viewer/config.hh>
#include <ostream>

namespace gepetto {

typedef char const* (*QStringInsertFunction)(QString const& s);

char const* use_toLocal8Bit(QString const& s) GEPETTO_VIEWER_LOCAL;
char const* use_toUtf8(QString const& s) GEPETTO_VIEWER_LOCAL;
char const* use_toLatin1(QString const& s) GEPETTO_VIEWER_LOCAL;
std::ostream& operator<<(std::ostream& out,
                         QStringInsertFunction fun) GEPETTO_VIEWER_LOCAL;
std::ostream& operator<<(std::ostream& out,
                         QString const& s) GEPETTO_VIEWER_LOCAL;
QStringInsertFunction& insertFunction() GEPETTO_VIEWER_LOCAL;

inline char const* use_toLocal8Bit(QString const& s) {
  return s.toLocal8Bit().constData();
}

inline char const* use_toUtf8(QString const& s) {
  return s.toUtf8().constData();
}

inline char const* use_toLatin1(QString const& s) {
  return s.toLatin1().constData();
}

inline std::ostream& operator<<(std::ostream& out, QStringInsertFunction fun) {
  insertFunction() = fun;
  return out;
}

inline std::ostream& operator<<(std::ostream& out, QString const& s) {
  return out << insertFunction()(s);
}

void setLogFile(const char* filename) GEPETTO_VIEWER_LOCAL;

std::ostream& log() GEPETTO_VIEWER_LOCAL;
}  // namespace gepetto
