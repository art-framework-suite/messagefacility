cet_find_package(Catch2 REQUIRED PRIVATE)
cet_make_library(LIBRARY_NAME MF_catch_main STATIC SOURCE catch_main.cc
  LIBRARIES PRIVATE Catch2::Catch2 fhiclcpp::fhiclcpp)
add_custom_command(TARGET MF_catch_main POST_BUILD
  COMMAND strip -S $<TARGET_FILE:MF_catch_main>
  COMMENT "Stripping messagefacility-Catch2 main() library"
  )

# install files
install_headers()
install_source()
