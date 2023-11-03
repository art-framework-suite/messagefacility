# ======================================================================
#
# Testing
#
# ======================================================================

cet_enable_asserts()
include(CetTest)

# Put everything in a different export set.
cet_register_export_set(SET_NAME Testing NAMESPACE messagefacility_test SET_DEFAULT)

add_subdirectory(Integration)
if (BUILD_TESTING)
  add_subdirectory(Utilities)
  add_subdirectory(MessageService)
endif()
