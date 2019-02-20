MACRO(ADD_OSGQT_DEPENDENCY)
  IF(${OPENSCENEGRAPH_VERSION} VERSION_GREATER "3.5.5")
    ADD_REQUIRED_DEPENDENCY("openscenegraph-osgQt5")
    IF(NOT DEFINED PROJECT_USE_QT4)
      SET(PROJECT_USE_QT4 FALSE CACHE BOOL "Use Qt4 instead of Qt5")
    ENDIF(NOT DEFINED PROJECT_USE_QT4)
    IF(PROJECT_USE_QT4)
      MESSAGE(FATAL_ERROR "Cannot use OSG >= 3.5.5 and Qt4")
    ENDIF(PROJECT_USE_QT4)
  ELSE()
    ADD_REQUIRED_DEPENDENCY("openscenegraph-osgQt")

    IF(NOT DEFINED PROJECT_USE_QT4)
      FILE(READ "${OPENSCENEGRAPH_OSGQT_INCLUDEDIR}/osgQt/Version"
        osgqt_version_content
        LIMIT 500)
      STRING(FIND ${osgqt_version_content} "#define OSGQT_QT_VERSION 4" osgqt_qt_version4_idx)
      STRING(FIND ${osgqt_version_content} "#define OSGQT_QT_VERSION 5" osgqt_qt_version5_idx)
      IF(osgqt_qt_version4_idx EQUAL -1 AND osgqt_qt_version5_idx EQUAL -1)
      ELSEIF(osgqt_qt_version4_idx EQUAL -1 )
        # QT5 only
        SET(PROJECT_USE_QT4 FALSE CACHE BOOL "Use Qt4 instead of Qt5")
      ELSEIF(osgqt_qt_version5_idx EQUAL -1 )
        # QT4 only
        SET(PROJECT_USE_QT4 TRUE CACHE BOOL "Use Qt4 instead of Qt5")
      ELSE()
        # Both found.
        MESSAGE(FATAL_ERROR "Could not compute Qt version. Please set PROJECT_USE_QT4 variable manually")
      ENDIF()
    ENDIF(NOT DEFINED PROJECT_USE_QT4)
  ENDIF()
ENDMACRO(ADD_OSGQT_DEPENDENCY)
