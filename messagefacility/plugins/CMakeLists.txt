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

include(mfPlugin)
include(mfStatsPlugin)

# Output plugins.
foreach (plugin cout cerr file syslog sqlite)
  mfPlugin(${plugin})
endforeach()

mfPlugin(stringstream
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
  mfStatsPlugin(${plugin})
endforeach()

target_link_libraries(messagefacility_plugins_file_mfStatsPlugin PRIVATE cetlib::cetlib)

install_headers()
install_source(SUBDIRS private)
