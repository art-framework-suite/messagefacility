// vim: set sw=2 expandtab :

#include "messagefacility/plugins/mfStatsPlugin.h"

#include "fhiclcpp/types/AllowedConfigurationMacro.h"

#include <iostream>
#include <memory>

using namespace std;

MAKE_MFSTATSPLUGIN_START(, pset)
{
  return make_unique<MFSTATSPLUGIN_RESULT_BASE>(pset, cout);
}
MAKE_PLUGIN_END

FHICL_PROVIDE_ALLOWED_CONFIGURATION(MFSTATSPLUGIN_RESULT_BASE)
