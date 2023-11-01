cet_test_env(CLEAR "FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/fcl")
cet_transitive_paths(LIBRARY_DIR BINARY IN_TREE)
cet_test_env_prepend(CET_PLUGIN_PATH ${TRANSITIVE_PATHS_WITH_LIBRARY_DIR})

include(MessagefacilityPlugins)
cet_make_exec(NAME ELdestinationTester
              LIBRARIES PRIVATE
                MF_MessageLogger
                fhiclcpp::types
                fhiclcpp::fhiclcpp
                cetlib::cetlib
                Boost::filesystem
                Boost::program_options)

if (NOT BUILD_TESTING)
  return()
endif()

cet_test(Presence_init_t LIBRARIES PRIVATE MF_MessageLogger fhiclcpp::fhiclcpp)

cet_test(WildcardDebug_t
         LIBRARIES PRIVATE
           MF_MessageLogger
           fhiclcpp::types
           fhiclcpp::fhiclcpp
           Boost::filesystem
           Boost::program_options
         TEST_ARGS -c WildcardDebug_t.fcl
         REF ${CMAKE_CURRENT_SOURCE_DIR}/WildcardDebug_t-ref.txt)

cet_build_plugin(Issue17457TestDestination mfPlugin NO_INSTALL LIBRARIES
  REG MF_MessageLogger fhiclcpp::types)

foreach (N IN ITEMS 01 02) # 03 04 05)
  set(test_stem Issue17457_${N})
  cet_test(${test_stem}_t HANDBUILT
           TEST_EXEC ELdestinationTester
           TEST_ARGS -c ${test_stem}.fcl
           OUTPUT_FILTERS DEFAULT ${CMAKE_CURRENT_SOURCE_DIR}/Issue17457-filter
           REF ${CMAKE_CURRENT_SOURCE_DIR}/${test_stem}-ref.out
           ${CMAKE_CURRENT_SOURCE_DIR}/${test_stem}-ref.err)
endforeach()
