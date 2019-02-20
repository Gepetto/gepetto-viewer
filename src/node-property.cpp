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

#include <sstream>

namespace gepetto {
namespace viewer {
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
  MetaEnum* lightingModeEnum  ()
  {
    static MetaEnum lm;
    if (lm.type.size() == 0) {
      lm.type = "LightingMode";
      lm.names .push_back ("ON" ); lm.values.push_back (LIGHT_INFLUENCE_ON );
      lm.names .push_back ("OFF"); lm.values.push_back (LIGHT_INFLUENCE_OFF);
    }
    return &lm;
  }
  MetaEnum* glImmediateModeEnum ()
  {
    static MetaEnum lm;
    if (lm.type.size() == 0) {
      lm.type = "GLImmediateMode";
      lm.names.push_back ("GL_LINES"         ); lm.values.push_back (GL_LINES         );
      lm.names.push_back ("GL_POINTS"        ); lm.values.push_back (GL_POINTS        );
      lm.names.push_back ("GL_LINE_STRIP"    ); lm.values.push_back (GL_LINE_STRIP    );
      lm.names.push_back ("GL_LINE_LOOP"     ); lm.values.push_back (GL_LINE_LOOP     );
      lm.names.push_back ("GL_POLYGON"       ); lm.values.push_back (GL_POLYGON       );
      lm.names.push_back ("GL_QUADS"         ); lm.values.push_back (GL_QUADS         );
      lm.names.push_back ("GL_QUAD_STRIP"    ); lm.values.push_back (GL_QUAD_STRIP    );
      lm.names.push_back ("GL_TRIANGLE_STRIP"); lm.values.push_back (GL_TRIANGLE_STRIP);
      lm.names.push_back ("GL_TRIANGLES"     ); lm.values.push_back (GL_TRIANGLES     );
      lm.names.push_back ("GL_TRIANGLE_FAN"  ); lm.values.push_back (GL_TRIANGLE_FAN  );
    }
    return &lm;
  }

  bool EnumProperty::set(const int& value)
  {
    const MetaEnum& me = *metaEnum();
    for (std::size_t i = 0; i < me.values.size(); ++i)
      if (me.values[i] == value)
        return IntProperty::set(value);
    std::ostringstream oss;
    oss << "Invalid value " << value << " for enum " << me.type << ". "
      "Possible values are ";
    for (std::size_t i = 0; i < me.values.size(); ++i)
      oss << me.names[i] << " (" << me.values[i] << "), ";
    throw std::invalid_argument(oss.str());
    return false;
  }
} /* namespace viewer */

} /* namespace gepetto */
