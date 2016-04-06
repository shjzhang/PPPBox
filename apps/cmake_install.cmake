# Install script for directory: /home/kemin/develop/PPPBOX/apps

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/pppbox")
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

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/kemin/develop/PPPBOX/apps/checktools/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/clocktools/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/converters/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/DataAvailability/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/differential/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/difftools/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/filetools/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/geomatics/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/ionosphere/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/troposphere/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/mergetools/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/MDPtools/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/rfw/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/swrx/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/receiver/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/multipath/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/performance/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/positioning/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/reszilla/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/Rinextools/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/time/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/visibility/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/dev/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/ephint/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/ssc/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/cc2noncc/cmake_install.cmake")
  INCLUDE("/home/kemin/develop/PPPBOX/apps/rnxfilter/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

