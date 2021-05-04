#include "stringstream.h"
#include "private/stringstream_streams.h"

#include "messagefacility/Utilities/exception.h"

#include <iostream>
#include <sstream>
#include <string>

std::map<std::string, cet::ostream_handle> mf::priv::streams{};

using namespace std;
using mf::priv::streams;

ostringstream&
mf::getStringStream(string const& psetName)
{
  auto const I = streams.find(psetName);
  if (I != streams.end()) {
    return dynamic_cast<ostringstream&>(static_cast<ostream&>(I->second));
  }
  throw Exception(errors::Configuration, "mf::getStringStream:")
    << "Unable to find specified messagefacility logging stream \"" << psetName
    << ":\" please ensure it has been configured as a destination.";
}
