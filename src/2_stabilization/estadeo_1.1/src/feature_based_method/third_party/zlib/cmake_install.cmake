# Install script for directory: /home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
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

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/third_party/zlib" TYPE FILE FILES
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/CMakeLists.txt"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/ChangeLog"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/INDEX"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/README"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/README.ipol"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/zconf.h.cmakein"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/zconf.h.in"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/zlib2ansi"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/zlib.3"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/zlib.map"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/zlib.pc.in"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/zconf.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/zlib.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/crc32.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/deflate.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/gzguts.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/inffast.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/inffixed.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/inflate.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/inftrees.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/trees.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/zutil.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/adler32.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/compress.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/crc32.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/deflate.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/gzclose.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/gzlib.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/gzread.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/gzwrite.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/inflate.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/infback.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/inftrees.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/inffast.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/trees.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/uncompr.c"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/zutil.c"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/third_party/zlib/win32" TYPE FILE FILES
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/win32/zlib1.rc"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/zlib/win32/zlib.def"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

