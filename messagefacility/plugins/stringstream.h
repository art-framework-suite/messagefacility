#ifndef messagefacility_plugins_stringstream_h
#define messagefacility_plugins_stringstream_h
// vim: set sw=2 expandtab :

#include <sstream>
#include <string>

namespace mf {

  std::ostringstream& getStringStream(std::string const& psetName);

} // namespace mf

#endif /* messagefacility_plugins_stringstream_h */

// Local Variables:
// mode: c++
// End:
