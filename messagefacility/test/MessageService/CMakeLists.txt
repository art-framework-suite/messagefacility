cet_test_env(CLEAR "FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/fcl")
cet_transitive_paths(LIBRARY_DIR BINARY IN_TREE)
cet_test_env_prepend(CET_PLUGIN_PATH ${TRANSITIVE_PATHS_WITH_LIBRARY_DIR})

set(SUCCESSFUL_CONFIGS
      debugModules_destination_file
      destination_cerr
      destination_cerr_wStatistics
      destination_cout
      destination_default
      destination_file_categories
      destination_file_duplicateDest
      destination_file
      destination_file_filenameOptions
      destination_file_lineLength
      destination_file_noLineBreaks
      destination_file_noTimeStamps
      destination_file_threshold
      destination_file_timestamp
      destination_file_useMilliseconds
      destination_file_wStatistics1
      destination_file_wStatistics2
      destination_file_wStatistics3
      destination_syslog
      empty
      messagefacility
      no_destinations
      sqlite_mfplugin_t
      statistics
      suppressDebug_destination_file
      suppressInfo_destination_file
      suppressWarning_destination_file)

foreach(testname IN LISTS SUCCESSFUL_CONFIGS)
  cet_test(${testname}_t HANDBUILT
    TEST_EXEC ELdestinationTester
    TEST_ARGS -c ${testname}.fcl)
endforeach()

cet_test(statistics_duplicateDest_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS -c statistics_duplicateDest.fcl
  TEST_PROPERTIES WILL_FAIL TRUE)
