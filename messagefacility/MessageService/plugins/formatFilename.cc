#include "messagefacility/MessageService/MessageLoggerDefaults.h"
#include "messagefacility/MessageService/plugins/formatFilename.h"

#include "fhiclcpp/ParameterSet.h"

using namespace mf::service;

namespace mfplugins {
  
  std::string formatFilename( const std::string& psetname,
                              const fhicl::ParameterSet& pset ) {
    
    // Determine the destination file name to use if no explicit filename is
    // supplied in the .fcl file.
    std::string filename_default = pset.get<std::string>("output", std::string() );
    
    if ( filename_default.empty() ) {
      auto messageLoggerDefaults = MessageLoggerDefaults( MessageLoggerDefaults::mode("grid") );
      filename_default = messageLoggerDefaults.output(psetname);
      if (filename_default.empty()) {
        filename_default  = psetname;
      }
    }

    std::string       filename           = pset.get<std::string>("filename", filename_default);
    std::string const explicit_extension = pset.get<std::string>("extension", std::string() );

    if ( !explicit_extension.empty() )
      {
        if (explicit_extension[0] == '.')
          filename = filename + explicit_extension;
        else
          filename = filename + "." + explicit_extension;
      }

    // Attach a default extension of .log if there is no extension on a file
    if (filename.find('.') == std::string::npos)
      filename += ".log";

    return filename;

  }
  
}
