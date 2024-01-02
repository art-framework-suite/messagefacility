include(CetMake)

find_package(Boost COMPONENTS filesystem program_options REQUIRED)
find_package(Catch2 REQUIRED EXPORT)
find_package(cetlib REQUIRED EXPORT)
find_package(cetlib_except REQUIRED EXPORT)
find_package(fhiclcpp REQUIRED EXPORT)

cet_make_library(LIBRARY_NAME MF_MessageLogger HEADERS_TARGET
  SOURCE
    MessageLogger/MessageLogger.cc
    MessageLogger/MFConfig.cc
    MessageService/ELdestination.cc
    MessageService/ELostreamOutput.cc
    MessageService/ELstatistics.cc
    Utilities/ELextendedID.cc
    Utilities/ELseverityLevel.cc
    Utilities/ErrorObj.cc
    Utilities/exception.cc
  LIBRARIES
  PUBLIC
    fhiclcpp::types
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
)

install_headers(SUBDIRS MessageLogger MessageService)
install_source(SUBDIRS MessageLogger MessageService)

add_subdirectory(Utilities)
add_subdirectory(plugins)

# N.B. ELdestinationTester should be built and installed even if
# BUILD_TESTING is FALSE (see test/CMakeLists.txt and
# test/Integration/CMakeLists.txt).
add_subdirectory(test)
