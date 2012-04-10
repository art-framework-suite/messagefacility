#
#  CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for project MessageFacility
#
#   cmake [-DCMAKE_INSTALL_PREFIX=/install/path] 
#         [-DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain/file]
#         /path/to/source/code

# Create a build directory and an install direcotory which are outside the source code directory
# Declare the install path from the cmake command line using either a relative or full path
#
#   cd .../path/to/build/directory
#   source .../path/to/MessageFacility/ups/setup_for_development
#   cmake -DCMAKE_INSTALL_PREFIX=/install/path $TOYCMAKE_SOURCE
#   make
#   make test
#   make install
#   make package
#

# use cmake 2.6 or later
cmake_minimum_required (VERSION 2.6)
# project name
project(MessageFacility)
# the ups convention is that product names are all lower case, but this is a choices
SET (product messagefacility)
SET (version v1_8_11c)
SET (qualifier a2)

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()
set(CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules")
set(CMAKE_MODULE_PATH $ENV{CETBUILDTOOLS_DIR}/Modules ${CMAKE_MODULE_PATH})
message(STATUS "module path is ${CMAKE_MODULE_PATH}" )

# because we want to move these libraries about,
# do not embed full path in shared libraries or executables
set (CMAKE_SKIP_RPATH)

ENABLE_TESTING()

# define some necessary modules
include(FindUpsPackage)
include(FindUpsBoost)
include(SetFlavorQual)
include(ParseUpsVersion)

#set package version from ups version
set_version_from_ups( ${version} )
#define flavorqual and flavorqual_dir
set_flavor_qual()


# these are minimum required versions, not the actual product versions
find_ups_product( cetbuildtools v1_7_4 )
find_ups_product( cpp0x v1_0_0 )
find_ups_product( cetlib v0_4_11 )
find_ups_product( fhiclcpp v2_11_0  )
find_ups_boost( v1_42_0 filesystem
                        thread
                        program_options )

# make sure gcc has been set
# note that gcc has no qualifier
SET ( GCC_VERSION $ENV{GCC_VERSION} )
IF (NOT GCC_VERSION)
    MESSAGE (FATAL_ERROR "gcc has not been setup")
ENDIF()
message(STATUS "GCC version is ${GCC_VERSION} " )

# Use cmake directive include_directories to define include search path

# We want to be able to #include "MyProduct/MyHeader.h" in source code,
# generated code, and user code.
# We achieve this by putting all headers into a MyProduct subdirectory  
# instead of an include subdirectory.  
# PROJECT_SOURCE_DIR is a cmake variable which points to the top of the
# source code tree.
include_directories ("${PROJECT_SOURCE_DIR}")

# Generated code will be created in the build directory, 
# so we also add PROJECT_BINARY_DIR to the include search path.
# This step may be omitted if there are no generated headers.
include_directories ("${PROJECT_BINARY_DIR}")

# let cmake define variables for external libraries and executables

# define CETLIB
find_library( CETLIB NAMES cetlib PATHS $ENV{CETLIB_LIB} )

# define FHICLCPP
find_library( FHICLCPP NAMES fhiclcpp PATHS $ENV{FHICLCPP_LIB} )

# Building  all libraries and binaries in a single lib and bin directory 
# is useful for a complex product like art.  
# You may not want to do this for smaller products.

#build all libraries in a single directory
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#build all executables in a single directory
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# The environment has been established, now generate the Makefiles.

subdirs(messagefacility)
subdirs(test)
subdirs(xcompile)
subdirs(ups)

# This step enables building the package tarball
# packaging utility
include(UseCPack)
