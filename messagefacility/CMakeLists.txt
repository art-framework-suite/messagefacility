include(CetMake)

cet_find_package(Boost PRIVATE COMPONENTS filesystem program_options REQUIRED)
cet_find_package(Catch2 PRIVATE REQUIRED)
cet_find_package(cetlib REQUIRED)
cet_find_package(cetlib_except REQUIRED)
cet_find_package(fhiclcpp REQUIRED)
cet_find_package(hep_concurrency PRIVATE REQUIRED)

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
  PRIVATE
    hep_concurrency::hep_concurrency
)

install_headers(SUBDIRS MessageLogger MessageService)
install_source(SUBDIRS MessageLogger MessageService)

add_subdirectory(Utilities)
add_subdirectory(plugins)

# Testing
add_subdirectory(test)
