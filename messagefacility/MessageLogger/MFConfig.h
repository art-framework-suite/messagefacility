#ifndef messagefacility_MessageLogger_MFConfig_h
#define messagefacility_MessageLogger_MFConfig_h
// vim: set sw=2 expandtab :

#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/OptionalDelegatedParameter.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/TableFragment.h"

#include <string>

namespace mf {

  struct MFDestinationConfig {
    struct Config {
      Config();
      fhicl::OptionalDelegatedParameter destinations;
    };

    ~MFDestinationConfig();
    MFDestinationConfig();
    MFDestinationConfig(MFDestinationConfig const&) = delete;
    MFDestinationConfig(MFDestinationConfig&&) = delete;
    MFDestinationConfig& operator=(MFDestinationConfig const&) = delete;
    MFDestinationConfig& operator=(MFDestinationConfig&&) = delete;
  };

  struct MFConfig {
    struct Config {
      Config();

      // Note: Now obsolete, not removed to preserve backwards compatibility.
      fhicl::Sequence<std::string> debugModules;
      // Note: Now obsolete, not removed to preserve backwards compatibility.
      fhicl::Sequence<std::string> suppressDebug;
      // Note: Now obsolete, not removed to preserve backwards compatibility.
      fhicl::Sequence<std::string> suppressInfo;
      // Note: Now obsolete, not removed to preserve backwards compatibility.
      fhicl::Sequence<std::string> suppressWarning;
      fhicl::TableFragment<MFDestinationConfig::Config> destinations;
    };

    using Parameters = fhicl::WrappedTable<Config>;
  };

} // namespace mf

#endif /* messagefacility_MessageLogger_MFConfig_h */

// Local Variables:
// mode: c++
// End:
