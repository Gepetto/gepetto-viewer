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

namespace PyQCustomPlot {
  /// \ingroup pluginlist
  /// Provide bindings of QCustomPlot library in PythonQt terminal.
  /// See QCustomPlotDecorator for the API available in Python.
  ///
  /// Add the following to your settings file to activate it.
  ///
  ///     [plugins]
  ///     libpyqcustomplot.so=true
  ///
  /// \par Example: Simple usage of QCustomPlot Python bindings.
  /// \include pyplugin/qcustomplot_example_1.py
  /// \sa See examples \ref pyplugin/qcustomplot_example_2.py
  class Plugin : public QObject, public gepetto::gui::PluginInterface {
    Q_OBJECT
    Q_INTERFACES (gepetto::gui::PluginInterface)
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    Q_PLUGIN_METADATA (IID "gepetto-viewer.pyqcustomplot")
#endif

    public:
      QString name () const { return QString("PyQCustomPlot"); }

      /// Calls \ref registerQCustomPlot
      void init();
  };
  /// \example pyplugin/qcustomplot_example_2.py
  /// Advanced example usage of QCustomPlot Python bindings.
} // namespace PyQCustomPlot
