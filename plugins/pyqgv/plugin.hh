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
#include <QtGlobal>
#include <gepetto/gui/plugin-interface.hh>

namespace PyQgv {
/// \ingroup pluginlist
/// Provide bindings of QGV library in PythonQt terminal.
/// Add the following to your settings file to activate it.
///
///     [plugins]
///     libpyqgv.so=true
///
class Plugin : public QObject, public gepetto::gui::PluginInterface {
  Q_OBJECT
  Q_INTERFACES(gepetto::gui::PluginInterface)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
  Q_PLUGIN_METADATA(IID "gepetto-viewer.pyqgv")
#endif

 public:
  QString name() const { return QString("PyQGV"); }

  /// Calls \ref registerQGV
  void init();
};
}  // namespace PyQgv
