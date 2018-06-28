#
#  CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for project MessageFacility
#
# Create a build directory and an install direcotory which are outside the source code directory
#  cd .../path/to/build/directory
#  source .../path/to/source/code/setup_for_development
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)

# Required to keep CMake happy.
cmake_minimum_required(VERSION ${CMAKE_VERSION} FATAL_ERROR)
# Actual version requirement.
cmake_policy(VERSION 3.4)

# Project information.
project(messagefacility VERSION 2.02.02 LANGUAGES CXX)

# cetmodules contains our cmake modules
find_package(cetmodules REQUIRED)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  EXTRA_CXX_FLAGS -Wnon-virtual-dtor -Wdelete-non-virtual-dtor
  )
cet_report_compiler_flags()

find_package(cetlib_except REQUIRED)
find_package(cetlib REQUIRED)
find_package(fhiclcpp REQUIRED)
find_package(hep_concurrency REQUIRED)
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
find_package(TBB COMPONENTS tbb REQUIRED)
find_package(Catch2 2.0.0 REQUIRED)

# SQLite
find_library(SQLITE3 NAMES sqlite3 REQUIRED)

# The environment has been established, now generate the Makefiles.

add_subdirectory(messagefacility)
add_subdirectory(perllib)
add_subdirectory(xcompile)
add_subdirectory(ups)
