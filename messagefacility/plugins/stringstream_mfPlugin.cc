// vim: set sw=2 expandtab :
#include "private/stringstream_streams.h"

#include "messagefacility/plugins/mfPlugin.h"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"

#include <memory>
#include <sstream>
#include <string>

using namespace std;
using cet::ostream_handle;
using mf::priv::streams;

MAKE_MFPLUGIN_START(psetName, pset)
{
  auto ret = streams.emplace(psetName, ostream_handle{ostringstream{}});
  return make_unique<MFPLUGIN_RESULT_BASE>(pset, ret.first->second);
}
MAKE_PLUGIN_END

FHICL_PROVIDE_ALLOWED_CONFIGURATION(MFPLUGIN_RESULT_BASE)
