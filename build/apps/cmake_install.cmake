# Install script for directory: /home/ww/pppbox/apps

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/ww/pppbox/build/apps/checktools/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/clocktools/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/converters/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/DataAvailability/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/differential/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/difftools/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/filetools/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/geomatics/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/ionosphere/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/mergetools/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/MDPtools/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/rfw/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/swrx/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/receiver/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/multipath/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/performance/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/positioning/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/reszilla/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/Rinextools/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/time/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/visibility/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/dev/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/ephint/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/ssc/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/cc2noncc/cmake_install.cmake")
  include("/home/ww/pppbox/build/apps/rnxfilter/cmake_install.cmake")

endif()

