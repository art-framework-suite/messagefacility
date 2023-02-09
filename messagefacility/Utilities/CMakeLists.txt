cet_make_library(LIBRARY_NAME MF_catch_main STATIC STRIP_LIBS
  SOURCE catch_main.cc
  LIBRARIES PRIVATE fhiclcpp::fhiclcpp Catch2::Catch2WithMain)

# install files
install_headers()
install_source()
