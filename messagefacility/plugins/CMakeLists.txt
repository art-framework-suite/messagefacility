cet_register_export_set(SET_NAME PluginTypes NAMESPACE art_plugin_types)

# Easy generation of mfPlugins
cet_make_library(LIBRARY_NAME mfPlugin INTERFACE
  EXPORT_SET PluginTypes
  SOURCE mfPlugin.h
  LIBRARIES INTERFACE
  art_plugin_support::plugin_config_macros # Courtesy, for
                                    # AllowedConfigurationMacros.h
  art_plugin_support::support_macros
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
)

# Easy generation of mfStatsPlugins
cet_make_library(LIBRARY_NAME mfStatsPlugin INTERFACE
  EXPORT_SET PluginTypes
  SOURCE mfStatsPlugin.h
  LIBRARIES INTERFACE
  art_plugin_support::plugin_config_macros # Courtesy, for
                                    # AllowedConfigurationMacros.h
  art_plugin_support::support_macros
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
)

include(BasicPlugin)
foreach(type IN ITEMS mfPlugin mfStatsPlugin)
  cet_make_plugin_builder(${type} ${type} Modules
    LIBRARIES REG art_plugin_types::${type})
endforeach()

cet_collect_plugin_builders(Modules MessagefacilityPlugins)
include(MessagefacilityPlugins)

# Output plugins.
foreach (plugin cout cerr file syslog sqlite)
  cet_build_plugin(${plugin} mfPlugin)
endforeach()

cet_build_plugin(stringstream mfPlugin
  LIBRARIES
  PRIVATE messagefacility::MF_MessageLogger cetlib::cetlib
  REG cetlib::cetlib
)

# These plugins additionally use cet::ostream_handle.
foreach (plugin cerr cout file)
  target_link_libraries(messagefacility_plugins_${plugin}_mfPlugin PRIVATE cetlib::cetlib)
endforeach()

target_link_libraries(messagefacility_plugins_sqlite_mfPlugin PRIVATE cetlib::sqlite)

# Statistics plugins.
foreach (plugin cout cerr file)
  cet_build_plugin(${plugin} mfStatsPlugin)
endforeach()

target_link_libraries(messagefacility_plugins_file_mfStatsPlugin PRIVATE cetlib::cetlib)

install_headers()
install_source(SUBDIRS private)
