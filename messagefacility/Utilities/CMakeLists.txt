cet_make_library(LIBRARY_NAME MF_catch_main STATIC STRIP_LIBS
  SOURCE catch_main.cc
  LIBRARIES PRIVATE fhiclcpp::fhiclcpp)
# FIXME: Not a link because we don't want to force users of
#        messagefacility to have to have Catch2 available if they're not
#        using MF_catch_main - need to get components working.
target_include_directories(MF_catch_main
  PRIVATE "$<TARGET_PROPERTY:Catch2::Catch2,INTERFACE_INCLUDE_DIRECTORIES>")

# install files
install_headers()
install_source()
