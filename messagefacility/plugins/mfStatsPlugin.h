#ifndef messagefacility_plugins_mfStatsPlugin_h
#define messagefacility_plugins_mfStatsPlugin_h
// vim: set sw=2 expandtab :

#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/ProvideMakePluginMacros.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELstatistics.h"

#include <memory>
#include <string>

#define MFSTATSPLUGIN_RESULT_BASE mf::service::ELstatistics

#define MAKE_MFSTATSPLUGIN_START(pset_name, pset)                              \
  MAKE_PLUGIN_START(                                                           \
    auto, std::string const& pset_name, fhicl::ParameterSet const& pset)

CET_PROVIDE_FILE_PATH()
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

#endif /* messagefacility_plugins_mfStatsPlugin_h */

// Local Variables:
// mode: c++
// End:
