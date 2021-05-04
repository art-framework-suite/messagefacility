#ifndef messagefacility_plugins_private_stringstream_streams_h
#define messagefacility_plugins_private_stringstream_streams_h
// vim: set sw=2 expandtab :

#include "cetlib/ostream_handle.h"

#include <map>
#include <string>

namespace mf::priv {

  extern std::map<std::string, cet::ostream_handle> streams;

}
#endif /* messagefacility_plugins_private_stringstream_streams_h */

// Local Variables:
// mode: c++
// End:
