# Install script for directory: /home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/third_party/CppUnitLite" TYPE FILE FILES
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/CMakeLists.txt"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/Failure.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/SimpleString.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/Test.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/TestHarness.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/TestRegistry.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/TestResult.h"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/Failure.cpp"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/SimpleString.cpp"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/Test.cpp"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/TestRegistry.cpp"
    "/home/jsanchez/src/plein_phare/ipol_demo/app/estadeo/src/orsa/src/third_party/CppUnitLite/TestResult.cpp"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

