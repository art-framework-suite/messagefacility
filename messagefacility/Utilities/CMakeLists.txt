# FIXME: We ought to avoid forcing users of messagefacility to have to
#        have Catch2 available if they're not using MF_catch_main - need
#        to get components working.
cet_make_library(LIBRARY_NAME MF_catch_main STATIC STRIP_LIBS
  SOURCE catch_main.cc
  LIBRARIES PRIVATE
  fhiclcpp::fhiclcpp
  Catch2::Catch2WithMain
)

# install files
install_headers()
install_source()
