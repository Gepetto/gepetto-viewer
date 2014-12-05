# 
#  Copyright 2010 CNRS-LAAS
#  Author: Florent Lamiraux
# 

# Configure package.dox file
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/package.dox.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/package.dox
)


ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
  COMMAND ${DOXYGEN_EXECUTABLE} \"${CMAKE_CURRENT_BINARY_DIR}/package.dox\"
)

ADD_CUSTOM_TARGET(documentation ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/html/index.html)

#
# Install html documentation
#

# html files
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
  DESTINATION share/doc/${PROJECT_NAME})

# doxytag
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.doxytag
  DESTINATION share/doc/${PROJECT_NAME}/html)

# pictures
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pictures
  DESTINATION share/doc/${PROJECT_NAME}/html)
