// vim: set sw=2 expandtab :

#include "private/FileConfig.h"

#include "messagefacility/plugins/mfPlugin.h"

#include "cetlib/ostream_handle.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/TableFragment.h"

#include <fstream>
#include <memory>

using namespace std;
using cet::ostream_handle;
using namespace mf::service;

namespace mf {

  struct file_mfPluginConfig {
    struct Config {
      fhicl::TableFragment<ELostreamOutput::Config> ostream_dest;
      fhicl::TableFragment<priv::FileConfig> file_config;
    };

    using Parameters = fhicl::WrappedTable<Config>;
  };

} // namespace mf

namespace {

  auto
  makePlugin_(mf::file_mfPluginConfig::Parameters const& ps)
  {
    ostream_handle osh{ps().file_config().filename(),
                       ps().file_config().append() ? ios::app : ios::trunc};
    return make_unique<ELostreamOutput>(ps().ostream_dest(), std::move(osh));
  }

} // unnamed namespace

MAKE_MFPLUGIN_START(, pset)
{
  return makePlugin_(pset);
}
MAKE_PLUGIN_END

FHICL_PROVIDE_ALLOWED_CONFIGURATION(mf::file_mfPluginConfig)
