macro(ADD_QT_DEPENDENCY)
  if(PROJECT_USE_QT4)
    add_project_dependency(
      Qt4
      REQUIRED
      QtCore
      QtGui
      QtOpenGL
      QtNetwork
      QtXml)
    set(PROJECT_QT_LIBS Qt4::QtCore Qt4::QtGui Qt4::QtOpenGL Qt4::QtNetwork
                        Qt4::QtXml)
    set(GEPETTO_VIEWER_QTVERSION
        "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
    include(${QT_USE_FILE})
    set(PACKAGE_EXTRA_MACROS "include(${QT_USE_FILE})\n${PACKAGE_EXTRA_MACROS}")
  else()
    add_project_dependency(
      Qt5
      REQUIRED
      COMPONENTS
      Xml
      Core
      Widgets
      Gui
      OpenGL
      Network
      Concurrent)
    foreach(
      component
      "Xml"
      "Core"
      "Widgets"
      "Gui"
      "OpenGL"
      "Network"
      "Concurrent")
      list(APPEND PROJECT_QT_LIBS Qt5::${component})
    endforeach()
    set(GEPETTO_VIEWER_QTVERSION "${Qt5Core_VERSION}")
  endif()
  set(PKG_CONFIG_EXTRA
      "qtversion=${GEPETTO_VIEWER_QTVERSION}\n${PKG_CONFIG_EXTRA}")
  set(PACKAGE_EXTRA_MACROS
      "set(GEPETTO_VIEWER_QTVERSION ${GEPETTO_VIEWER_QTVERSION})\n${PACKAGE_EXTRA_MACROS}"
  )

  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
endmacro(ADD_QT_DEPENDENCY)
