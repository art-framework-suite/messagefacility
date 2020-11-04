find_ups_product(catch) ### MIGRATE-ACTION-RECOMMENDED: use cet_find_package() with target semantics for linking
cet_make_library(LIBRARY_NAME MF_catch_main STATIC SOURCE catch_main.cc
  LIBRARIES PRIVATE Catch2::Catch2)
add_custom_command(TARGET MF_catch_main POST_BUILD
  COMMAND strip -S $<TARGET_FILE:MF_catch_main>
  COMMENT "Stripping messagefacility-Catch2 main() library"
  )

# install files
install_headers()
install_source()
