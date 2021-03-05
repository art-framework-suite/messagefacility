#ifndef messagefacility_plugins_mfPlugin_h
#define messagefacility_plugins_mfPlugin_h
// vim: set sw=2 expandtab :

#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/ProvideMakePluginMacros.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"

#include <memory>
#include <string>

#define MFPLUGIN_RESULT_BASE mf::service::ELostreamOutput

#define MAKE_MFPLUGIN_START(pset_name, pset)                       \
  MAKE_PLUGIN_START(auto,                                          \
                    std::string const& pset_name,                  \
                    fhicl::ParameterSet const& pset)
#define MAKE_MFPLUGIN_END(MAKE_PLUGIN_END)

CET_PROVIDE_FILE_PATH()
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

#endif /* messagefacility_plugins_mfPlugin_h */

// Local Variables:
// mode: c++
// End:
