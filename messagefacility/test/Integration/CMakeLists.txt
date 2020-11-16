# ======================================================================
#
# Testing script
#
# ======================================================================
include(BasicPlugin)

cet_find_package(Boost COMPONENTS program_options REQUIRED PRIVATE)

cet_make_exec(NAME ELdestinationTester
              LIBRARIES PRIVATE
                MF_MessageLogger
                fhiclcpp::fhiclcpp
                Boost::filesystem
                Boost::program_options)

cet_test(Presence_init_t LIBRARIES PRIVATE MF_MessageLogger fhiclcpp::fhiclcpp)

cet_test(WildcardDebug_t
         LIBRARIES PRIVATE
           MF_MessageLogger
           fhiclcpp::fhiclcpp
           Boost::program_options
         DATAFILES fcl/WildcardDebug_t.fcl
         TEST_ARGS -c WildcardDebug_t.fcl
         REF ${CMAKE_CURRENT_SOURCE_DIR}/WildcardDebug_t-ref.txt)

basic_plugin(Issue17457TestDestination mfPlugin
             LIBRARIES PRIVATE
             MF_MessageLogger
             fhiclcpp::fhiclcpp
             Boost::filesystem
             NO_INSTALL)

foreach (N 01 02) # 03 04 05)
  set(test_stem Issue17457_${N})
  cet_test(${test_stem}_t HANDBUILT
           TEST_EXEC ELdestinationTester
           TEST_ARGS -c ${test_stem}.fcl
           DATAFILES fcl/${test_stem}.fcl
           OUTPUT_FILTERS DEFAULT ${CMAKE_CURRENT_SOURCE_DIR}/Issue17457-filter
           REF ${CMAKE_CURRENT_SOURCE_DIR}/${test_stem}-ref.out
           ${CMAKE_CURRENT_SOURCE_DIR}/${test_stem}-ref.err)
endforeach()
