# Find PythonQt
#
# Sets PYTHONQT_FOUND, PYTHONQT_INCLUDE_DIR, PYTHONQT_LIBRARY,
# PYTHONQT_QTALL_LIBRARY, PYTHONQT_LIBRARIES
#

if(NOT EXISTS "${PYTHONQT_INSTALL_DIR}")
  find_path(
    PYTHONQT_INSTALL_DIR
    NAMES include/PythonQt/PythonQt.h include/PythonQt5/PythonQt.h
    DOC "Directory where PythonQt was installed.")
endif()
# XXX Since PythonQt 3.0 is not yet cmakeified, depending on how PythonQt is
# built, headers will not always be installed in "include/PythonQt". That is why
# "src" is added as an option. See [1] for more details. [1]
# https://github.com/commontk/CTK/pull/538#issuecomment-86106367
find_path(
  PYTHONQT_INCLUDE_DIR PythonQt.h
  PATHS "${PYTHONQT_INSTALL_DIR}/include/PythonQt"
        "${PYTHONQT_INSTALL_DIR}/include/PythonQt5"
  DOC "Path to the PythonQt include directory")
find_path(
  PYTHONQT_QTALL_INCLUDE_DIR PythonQt_QtAll.h
  PATHS "${PYTHONQT_INSTALL_DIR}/include/PythonQt"
        "${PYTHONQT_INSTALL_DIR}/include/PythonQt5"
  PATH_SUFFIXES "extensions/PythonQt_QtAll"
  DOC "Path to the PythonQt QtAll extension include directory")

set(PYTHONQT_INCLUDE_DIR ${PYTHONQT_INCLUDE_DIR} ${PYTHONQT_QTALL_INCLUDE_DIR})

set(PYTHONQT_LIBRARIES)
if(PROJECT_USE_QT4)
  set(PYTHONQT_LIBRARIES_SUFFIX
      "-Qt4-Python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")
  set(PYTHONQT_LIBRARIES_SUFFIX_MAJOR "-Qt4-Python${PYTHON_VERSION_MAJOR}")
else(PROJECT_USE_QT4)
  set(PYTHONQT_LIBRARIES_SUFFIX
      "-Qt5-Python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")
  set(PYTHONQT_LIBRARIES_SUFFIX_MAJOR "-Qt5-Python${PYTHON_VERSION_MAJOR}")
endif(PROJECT_USE_QT4)

macro(_SEARCH_FOR COMPONENT)
  string(TOUPPER ${COMPONENT} _COMP_UPPERCASE)
  find_library(
    ${_COMP_UPPERCASE}_LIBRARY
    NAMES ${COMPONENT} "${COMPONENT}${PYTHONQT_LIBRARIES_SUFFIX}"
          "${COMPONENT}${PYTHONQT_LIBRARIES_SUFFIX_MAJOR}"
    PATHS "${PYTHONQT_INSTALL_DIR}/lib"
    DOC "The ${COMPONENT} library.")
  if(NOT ${${_COMP_UPPERCASE}_LIBRARY} STREQUAL
     "${_COMP_UPPERCASE}_LIBRARY-NOTFOUND")
    set(${COMPONENT}_FOUND TRUE)
    set(PYTHONQT_LIBRARIES ${PYTHONQT_LIBRARIES} ${${_COMP_UPPERCASE}_LIBRARY})
  endif(NOT ${${_COMP_UPPERCASE}_LIBRARY} STREQUAL
        "${_COMP_UPPERCASE}_LIBRARY-NOTFOUND")

  mark_as_advanced(${_COMP_UPPERCASE}_LIBRARY)
endmacro(_SEARCH_FOR COMP)

_search_for(PythonQt)

foreach(_COMPONENT_SHORT ${PythonQt_FIND_COMPONENTS})
  set(_COMPONENT "PythonQt_${_COMPONENT_SHORT}")
  _search_for(${_COMPONENT})
endforeach(_COMPONENT_SHORT ${PythonQt_FIND_COMPONENTS})

mark_as_advanced(PYTHONQT_INSTALL_DIR)
mark_as_advanced(PYTHONQT_INCLUDE_DIR)
mark_as_advanced(PYTHONQT_LIBRARIES)

# All upper case _FOUND variable is maintained for backwards compatibility.
set(PythonQt_FOUND 0)
if(PYTHONQT_INCLUDE_DIR AND PYTHONQT_LIBRARIES)
  # Currently CMake'ified PythonQt only supports building against a python
  # Release build.
  add_definitions(-DPYTHONQT_USE_RELEASE_PYTHON_FALLBACK)
  set(PythonQt_FOUND 1)
endif()
set(PYTHONQT_FOUND ${PythonQt_FOUND})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  PythonQt
  REQUIRED_VARS PYTHONQT_LIBRARY PYTHONQT_INCLUDE_DIR
  HANDLE_COMPONENTS)
