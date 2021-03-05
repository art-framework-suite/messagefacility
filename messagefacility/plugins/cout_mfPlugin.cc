// vim: set sw=2 expandtab :

#include "messagefacility/plugins/mfPlugin.h"

#include "cetlib/ostream_handle.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"

#include <iostream>
#include <memory>
#include <string>

using namespace std;
using cet::ostream_handle;

MAKE_MFPLUGIN_START(, pset)
{
  return make_unique<MFPLUGIN_RESULT_BASE>(pset, ostream_handle{cout});
}
MAKE_PLUGIN_END

FHICL_PROVIDE_ALLOWED_CONFIGURATION(MFPLUGIN_RESULT_BASE)
