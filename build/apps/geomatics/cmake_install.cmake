# Install script for directory: /Users/shjzhang/Documents/Develop/rocket/pppbox/apps/geomatics

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/Users/shjzhang/Documents/Develop/bin/pppbox")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/Users/shjzhang/Documents/Develop/rocket/pppbox/build/apps/geomatics/cycleslips/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/rocket/pppbox/build/apps/geomatics/JPLeph/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/rocket/pppbox/build/apps/geomatics/kalman/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/rocket/pppbox/build/apps/geomatics/relposition/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/rocket/pppbox/build/apps/geomatics/robust/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

