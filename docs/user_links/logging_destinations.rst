Logging destinations
====================

All messagefacility destinations are implemented as plugin libraries. Currently supported destinations include:

======      =============================================
cout        configuration details
cerr        configuration details
file        configuration details
syslog      `configuration details <syslog_config.html>`_ 
sqlite      `configuration details <sqlite_config.html>`_       
======      =============================================   


Additional destinations can be added via the plugin definition mechanism described below.


User-defined logging destinations
---------------------------------

Messages are logged whenever the ELdestination::log function is called (done in the MessageLogger destructor). The implementation of log looks like:::

    void ELdestination::log( mf::ErrorObj & msgObj )  {
    
      if ( !passLogMsgThreshold  ( msgObj ) ) return;
    
      std::ostringstream payload;
    
      // Overridable functions
      fillPrefix( payload, msgObj );
      fillUsrMsg( payload, msgObj );
      fillSuffix( payload, msgObj );
      routePayload( payload, msgObj );
    
      msgObj.setReactedTo ( true );
    
      if ( userWantsStats && passLogStatsThreshold( msgObj ) ) stats.log( msgObj );
    
    }


The four overridable (i.e. virtual) functions above, are not pure, meaning that they do not have to be overridden by a derived class. 
Creating your own messagefacility plugin requires a file (say "newplugin_mfPlugin.cc") with a class declaration similar to:::

    #include "fhiclcpp/ParameterSet.h" 
    #include "messagefacility/MessageLogger/MessageDrop.h" 
    #include "messagefacility/MessageService/ELdestination.h" 
    
    #include <memory>
    #include <sstream>
    
    namespace newdest {
    
      class pluginName : public mf::service::ELdestination {
    
        pluginName( const fhicl::ParameterSet& pset ) : mf::service::ELdestination( pset ) {
          // c'tor implementation
        }
    
        // Set of virtual functions that can be overridden
        virtual void fillPrefix  (       std::ostringstream & oss, const ErrorObj & msg ) override;
        virtual void fillUsrMsg  (       std::ostringstream & oss, const ErrorObj & msg ) override;
        virtual void fillSuffix  (       std::ostringstream & oss, const ErrorObj & msg ) override;
        virtual void routePayload( const std::ostringstream & oss, const ErrorObj & msg ) override;
    
      };
    
      // Define overridden functions here
      // ...
    
    }
    
    // Make plugin destination
    
    extern "C" {
      auto makePlugin( const std::string& optFileName, const fhicl::ParameterSet & pset ) {
        return std::make_unique<newdest::pluginName>( pset );
      }
    }


In order to preserve the semantics, it is best for the users to conform to the descriptions of the function name when providing implementations.

Only plugins that have the mfPlugin suffix -- i.e. "newplugin_mfPlugin.cc" are loaded by the messagefacility. The plugin destination is chosen by the FHiCL configuration:::

    message: {
      destinations: {
    
        dest1 : {
          type : newplugin
          // ...
          // + whatever parameters are needed by 'newplugin_mfPlugin.cc'
        }
    
      }
    }


