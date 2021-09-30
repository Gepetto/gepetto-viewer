# Graphical Interface for Pinocchio and hpp

[![Building Status](https://travis-ci.org/gepetto/gepetto-viewer.svg?branch=master)](https://travis-ci.org/gepetto/gepetto-viewer)
[![Pipeline status](https://gitlab.laas.fr/gepetto/gepetto-viewer/badges/master/pipeline.svg)](https://gitlab.laas.fr/gepetto/gepetto-viewer/commits/master)
[![Coverage report](https://gitlab.laas.fr/gepetto/gepetto-viewer/badges/master/coverage.svg?job=doc-coverage)](http://projects.laas.fr/gepetto/doc/gepetto/gepetto-viewer/master/coverage/)

## Setup Conda Environment

To try this package in an [conda](https://conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html#creating-an-environment-with-commands) environment without compiling:
```sh
conda install gepetto-viewer gepetto-viewer-corba -c conda-forge
```

After installation, you can launch the viewer from a terminal with:
```sh
gepetto-gui
```

## Compilation Instructions

To compile this package, it is recommended to create a separate build
directory:
```sh
mkdir _build && cd _build
cmake [OPTIONS] ..
make install
```

Please note that CMake produces a `CMakeCache.txt` file which should
be deleted to reconfigure a package from scratch.

## Dependencies

The Graphical Interface software depends on several packages which
have to be available on your machine.

 - Libraries:
   - Qt 4 or 5
   - openscenegraph (version >= 3.2.0)
 - System tools:
   - CMake (>=2.6)
   - pkg-config
   - usual compilation tools (GCC/G++, make, etc.)

## Optional dependencies

- [PythonQt]: add a Python scripting interface to gepetto-viewer.
  The dependency can be made mandatory with CMake variable `GEPETTO_GUI_HAS_PYTHONQT`.
  When [PythonQt] is available:
  - [QGV]: for the bindings of QGV in the Python interface.
    The dependency can be made mandatory with CMake variable `BUILD_PY_QGV`.
  - [QCustomPlot]: for the bindings of QCustomPlot in the Python interface.
    The dependency can be made mandatory with CMake variable `BUILD_PY_QCUSTOM_PLOT`.

## Environment variables and plugins

When launching gepetto-gui the program tries to load the plugin omniorbserver.so. In order to specify the default path of the plugins,
the variable `GEPETTO_GUI_PLUGIN_DIRS` can be set using the following command:

```sh
export GEPETTO_GUI_PLUGIN_DIRS=${CMAKE_INSTALL_PREFIX}/lib
```

[PythonQt]: https://github.com/gepetto/pythonqt
[QGV]: https://github.com/gepetto/qgv
[QCustomPlot]: https://www.qcustomplot.com/
