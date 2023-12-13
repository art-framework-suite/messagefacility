cmake_minimum_required(VERSION 3.18.2 FATAL_ERROR)

find_package(cetmodules 2.13.00 REQUIRED)
project(messagefacility VERSION 2.10.04 LANGUAGES CXX)
include(CetCMakeEnv)
cet_cmake_env()

# Find our own CMake modules, and tell everyone else where to find them.
cet_cmake_module_directories(Modules BINARY)

cet_set_compiler_flags(DIAGS VIGILANT DWARF_VER 4
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
)

if(DEFINED ENV{CET_SANITIZE_ADDRESS} AND DEFINED ENV{GCC_FQ_DIR})
  find_file(ASAN_LIB libasan.so NO_DEFAULT_PATH PATHS $ENV{GCC_FQ_DIR}/lib64)
  cet_add_compiler_flags(CXX -fsanitize=address)
  cet_add_compiler_flags(C -fsanitize=address)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
  set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=address")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()

if(DEFINED ENV{CET_SANITIZE_THREAD} AND DEFINED ENV{GCC_FQ_DIR})
  find_file(TSAN_LIB libtsan.so NO_DEFAULT_PATH PATHS $ENV{GCC_FQ_DIR}/lib64)
  cet_add_compiler_flags(CXX -fsanitize=thread -D__SANITIZE_THREAD__)
  cet_add_compiler_flags(C -fsanitize=thread -D__SANITIZE_THREAD__)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=thread -D__SANITIZE_THREAD__")
  set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=thread -D__SANITIZE_THREAD__")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=thread -D__SANITIZE_THREAD__")
endif()

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

# The environment has been established, now generate the Makefiles.

add_subdirectory(messagefacility)
add_subdirectory(perllib)
add_subdirectory(xcompile)
add_subdirectory(tools/migration)

# This step enables building the package tarball
# packaging utility
cet_cmake_config(EXTRA_TARGET_VARS MF_MESSAGELOGGER MF_CATCH_MAIN)
