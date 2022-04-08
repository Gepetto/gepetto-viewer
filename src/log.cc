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

#include "log.hh"

#include <fstream>
#include <iostream>

namespace gepetto {

QStringInsertFunction& insertFunction() {
  // Default function to use to insert a QString.
  static QStringInsertFunction insertFunction(use_toLocal8Bit);
  return insertFunction;
}

std::ofstream& logfile() GEPETTO_VIEWER_LOCAL;

std::ofstream& logfile() {
  static std::ofstream ofs;
  return ofs;
}

void setLogFile(const char* filename) { logfile().open(filename); }

std::ostream& log() {
  if (logfile().good())
    return logfile();
  else
    return std::cout;
}
}  // namespace gepetto
