include(BasicPlugin)

set(mf_plugin_libraries
  MF_MessageLogger
  fhiclcpp::fhiclcpp
  Boost::filesystem)

# ordinary destination plugins
basic_plugin(cout mfPlugin LIBRARIES PRIVATE ${mf_plugin_libraries})
basic_plugin(cerr mfPlugin LIBRARIES PRIVATE ${mf_plugin_libraries})
basic_plugin(file mfPlugin LIBRARIES PRIVATE ${mf_plugin_libraries})
basic_plugin(syslog mfPlugin LIBRARIES PRIVATE ${mf_plugin_libraries})
basic_plugin(sqlite mfPlugin LIBRARIES PRIVATE ${mf_plugin_libraries} SQLite::SQLite3)
basic_plugin(stringstream mfPlugin LIBRARIES PRIVATE ${mf_plugin_libraries})

# statistics destination plugins
basic_plugin(cout mfStatsPlugin LIBRARIES PRIVATE ${mf_plugin_libraries})
basic_plugin(cerr mfStatsPlugin LIBRARIES PRIVATE ${mf_plugin_libraries})
basic_plugin(file mfStatsPlugin LIBRARIES PRIVATE ${mf_plugin_libraries})

install_headers()
install_source()
