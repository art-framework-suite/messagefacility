// vim: set sw=2 expandtab :

#include "private/FileConfig.h"

#include "messagefacility/plugins/mfStatsPlugin.h"

#include "cetlib/ostream_handle.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/TableFragment.h"

#include <fstream>
#include <memory>
#include <utility>

using cet::ostream_handle;
using namespace std;
using namespace mf::service;

namespace mf {

  struct file_mfStatsPluginConfig {
    struct Config {
      fhicl::TableFragment<ELstatistics::Config> stats_dest;
      fhicl::TableFragment<mf::priv::FileConfig> file_config;
    };

    using Parameters = fhicl::WrappedTable<Config>;
  };

} // namespace mf

namespace {

  auto
  makePlugin_(mf::file_mfStatsPluginConfig::Parameters const& ps)
  {
    ostream_handle osh{ps().file_config().filename(),
                       ps().file_config().append() ? ios::app : ios::trunc};
    return make_unique<MFSTATSPLUGIN_RESULT_BASE>(ps().stats_dest(), move(osh));
  }

} // unnamed namespace

MAKE_MFSTATSPLUGIN_START(, pset)
{
  return makePlugin_(pset);
}
MAKE_PLUGIN_END

FHICL_PROVIDE_ALLOWED_CONFIGURATION(mf::file_mfStatsPluginConfig)
