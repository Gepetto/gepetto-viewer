macro(ADD_OSGQT_DEPENDENCY)
  if(${OPENSCENEGRAPH_VERSION} VERSION_GREATER "3.5.5")
    if(NOT DEFINED PROJECT_USE_QT4)
      set(PROJECT_USE_QT4
          FALSE
          CACHE BOOL "Use Qt4 instead of Qt5")
    endif(NOT DEFINED PROJECT_USE_QT4)
    if(PROJECT_USE_QT4)
      set(OSGQT_DEPENDENCY openscenegraph-osgQt)
    else(PROJECT_USE_QT4)
      set(OSGQT_DEPENDENCY openscenegraph-osgQt5)
    endif(PROJECT_USE_QT4)
    add_required_dependency(${OSGQT_DEPENDENCY})
  else()
    set(OSGQT_DEPENDENCY openscenegraph-osgQt)
    add_required_dependency(${OSGQT_DEPENDENCY})

    if(NOT DEFINED PROJECT_USE_QT4)
      if(EXISTS "${OPENSCENEGRAPH_OSGQT_INCLUDEDIR}/osgQt/Version")
        file(READ "${OPENSCENEGRAPH_OSGQT_INCLUDEDIR}/osgQt/Version"
             osgqt_version_content LIMIT 500)
        string(FIND ${osgqt_version_content} "#define OSGQT_QT_VERSION 4"
                    osgqt_qt_version4_idx)
        string(FIND ${osgqt_version_content} "#define OSGQT_QT_VERSION 5"
                    osgqt_qt_version5_idx)
        if(osgqt_qt_version4_idx EQUAL -1 AND osgqt_qt_version5_idx EQUAL -1)

        elseif(osgqt_qt_version4_idx EQUAL -1)
          # QT5 only
          set(PROJECT_USE_QT4
              FALSE
              CACHE BOOL "Use Qt4 instead of Qt5")
        elseif(osgqt_qt_version5_idx EQUAL -1)
          # QT4 only
          set(PROJECT_USE_QT4
              TRUE
              CACHE BOOL "Use Qt4 instead of Qt5")
        else()
          # Both found.
          message(
            FATAL_ERROR
              "Could not compute Qt version. Please set PROJECT_USE_QT4 variable manually"
          )
        endif()
      else()
        # QT4 only
        set(PROJECT_USE_QT4
            TRUE
            CACHE BOOL "Use Qt4 instead of Qt5")
      endif()
    endif(NOT DEFINED PROJECT_USE_QT4)
  endif()
endmacro(ADD_OSGQT_DEPENDENCY)
