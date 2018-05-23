// Copyright (c) 2018, Joseph Mirabel
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

#include <gepetto/viewer/node-property.h>

namespace graphics {
  int MetaEnum::from_string (const std::string& s)
  {
    for (std::size_t i = 0; i < names.size(); ++i)
      if (s == names[i]) return values[i];
    throw std::invalid_argument("Unknown name of enum (" + type + "): " + s);
  }
  std::string MetaEnum::to_string (const int& v)
  {
    for (std::size_t i = 0; i < names.size(); ++i)
      if (v == values[i]) return names[i];
    throw std::invalid_argument("Unknown enum value (" + type + ")");
  }

  MetaEnum* visibilityModeEnum ()
  {
    static MetaEnum vm;
    if (vm.type.size() == 0) {
      vm.type = "VisibilityMode";
      vm.names .push_back ("ON"           ); vm.values.push_back (VISIBILITY_ON );
      vm.names .push_back ("ALWAYS_ON_TOP"); vm.values.push_back (ALWAYS_ON_TOP );
      vm.names .push_back ("OFF"          ); vm.values.push_back (VISIBILITY_OFF);
    }
    return &vm;
  }
  MetaEnum* wireFrameModeEnum  ()
  {
    static MetaEnum wm;
    if (wm.type.size() == 0) {
      wm.type = "VisibilityMode";
      wm.names .push_back ("FILL"              ); wm.values.push_back (FILL              );
      wm.names .push_back ("WIREFRAME"         ); wm.values.push_back (WIREFRAME         );
      wm.names .push_back ("FILL_AND_WIREFRAME"); wm.values.push_back (FILL_AND_WIREFRAME);
    }
    return &wm;
  }
} /* namespace graphics */