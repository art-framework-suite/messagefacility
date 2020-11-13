# ======================================================================
#
# Testing script
#
# ======================================================================

cet_test_env(CLEAR "FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}" OUT_VAR CET_TEST_ENV)

cet_test(Exception_t LIBRARIES MF_MessageLogger cetlib_except::cetlib_except)
cet_test(ExceptionDerived_t LIBRARIES MF_MessageLogger cetlib_except::cetlib_except)
cet_test(Simple_t LIBRARIES MF_MessageLogger cetlib_except::cetlib_except)
