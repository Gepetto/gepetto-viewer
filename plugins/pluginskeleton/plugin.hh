// Copyright (c) 2018 CNRS
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

#include <QObject>

#include <gepetto/gui/plugin-interface.hh>

/// \ingroup plugin
/// C++ skeleton plugin.
/// To add your own plugin, copy paste code in directory plugins/pluginskeleton.
class PluginSkeleton : public QObject, public gepetto::gui::PluginInterface {
  Q_OBJECT
    Q_INTERFACES (gepetto::gui::PluginInterface)

  public:
    QString name () const { return QString("PluginSkeleton"); }

  protected:
    void init();
};
