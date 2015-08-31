// ----------------------------------------------------------------------
//
// ELfwkJobReport.cc
//
//
// 1/10/06      mf, de  Created
//
// Changes:
//
//   1 - 3/22/06  mf  - in configure_dest()
//      Repaired the fact that destination limits for categories
//      were not being effective:
//      a) use values from the destination specific default PSet
//         rather than the overall default PSet to set these
//      b) when an explicit value has been set - either by overall default or
//         by a destination specific default PSet - set that limit or
//         timespan for that dest_ctrl via a "*" msgId.
//
// ----------------------------------------------------------------------


#include "messagefacility/MessageService/ELfwkJobReport.h"
#include "messagefacility/MessageLogger/ErrorObj.h"

// Possible Traces:
// #define ELfwkJobReportCONSTRUCTOR_TRACE
// #define ELfwkJobReportTRACE_LOG
// #define ELfwkJobReport_EMIT_TRACE

#include <iostream>
#include <fstream>

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // Constructors:
    // ----------------------------------------------------------------------

    ELfwkJobReport::ELfwkJobReport()
      : ELdestination( )
      , osh          ( std::make_unique<cet::ostream_observer>(std::cerr) )
      , charsOnLine  (0)
      , xid          ( )
    {

#ifdef ELfwkJobReportCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELfwkJobReport()\n";
#endif

      // Opening xml tag
      emit( "<FrameworkJobReport>\n", true );

    }  // ELfwkJobReport()


    ELfwkJobReport::ELfwkJobReport( std::ostream & os_ , bool /*emitAtStart*/ )
      : ELdestination       ( )
      , osh                 ( std::make_unique<cet::ostream_observer>(os_) )
      , charsOnLine         (0)
      , xid                 ( )
    {

#ifdef ELfwkJobReportCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELfwkJobReport( os )\n";
#endif

      // Opening xml tag
      emit( "<FrameworkJobReport>\n\n", true );

    }  // ELfwkJobReport()


    ELfwkJobReport::ELfwkJobReport( const ELstring & fileName, bool /*emitAtStart*/ )
      : ELdestination       ( )
      , osh                 ( std::make_unique<cet::ostream_owner>(fileName, std::ios::app) )
      , charsOnLine         (0)
      , xid                 ( )
    {

#ifdef ELfwkJobReportCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELfwkJobReport( " << fileName << " )\n";
#endif

      if ( osh && osh->stream() )  {
#ifdef ELfwkJobReportCONSTRUCTOR_TRACE
        std::cerr << "          Testing if os is owned\n";
#endif
#ifdef ELfwkJobReportCONSTRUCTOR_TRACE
        std::cerr << "          About to do first emit\n";
#endif
        // Opening xml tag
        emit( "<FrameworkJobReport>\n");
      } else  {
#ifdef ELfwkJobReportCONSTRUCTOR_TRACE
        std::cerr << "          Deleting os\n";
#endif
        osh = std::make_unique<cet::ostream_observer>(std::cerr);
#ifdef ELfwkJobReportCONSTRUCTOR_TRACE
        std::cerr << "          about to emit to cerr\n";
#endif
        // Opening xml tag
        emit( "<FrameworkJobReport>\n\n" );
      }

#ifdef ELfwkJobReportCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELfwkJobReport completed.\n";
#endif

    }  // ELfwkJobReport()


    ELfwkJobReport::~ELfwkJobReport()  {

#ifdef ELfwkJobReportCONSTRUCTOR_TRACE
      std::cerr << "Destructor for ELfwkJobReport\n";
#endif
    }  // ~ELfwkJobReport()


    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator:
    // ----------------------------------------------------------------------

    void ELfwkJobReport::log( mf::ErrorObj & msg )  {

#ifdef ELfwkJobReportTRACE_LOG
      std::cerr << "    =:=:=: Log to an ELfwkJobReport \n";
#endif

      xid = msg.xid();      // Save the xid.

      // Change log 1:  React ONLY to category FwkJob
      if (xid.id != "FwkJob") return;

      // See if this message is to be acted upon
      // (this is redundant if we are reacting only to FwkJob)
      // and add it to limits table if it was not already present:
      //
      if ( msg.xid().severity < threshold  )  return;

      if ( (xid.id == "BeginningJob")        ||
           (xid.id == "postBeginJob")        ||
           (xid.id == "preEventProcessing")  ||
           (xid.id == "preModule")           ||
           (xid.id == "postModule")          ||
           (xid.id == "postEventProcessing") ||
           (xid.id == "postEndJob")         ) return;
      if ( thisShouldBeIgnored(xid.module)  ) return;
      if ( ! stats.limits.add( msg.xid() )  ) return;

#ifdef ELfwkJobReportTRACE_LOG
      std::cerr << "    =:=:=: Limits table work done \n";
#endif

      // Output the prologue:
      //
      //emit ( "  <Report>\n" );
      //emit ( "    <Severity> " );
      //emit (xid.severity.getSymbol());
      //emit (" </Severity>\n");
      //emit ( "    <Category> ");
      //emit (xid.id);
      //emit ( " </Category>\n");
      //emit ( "    <Message> \n");

      //  emit( msg.idOverflow() ); this is how to get the rest of the category

#ifdef ELfwkJobReportTRACE_LOG
      std::cerr << "    =:=:=: Prologue done \n";
#endif

      // Output each item in the message:
      //
      if ( format.want( TEXT ) )  {
        ELlist_string::const_iterator it;
        for ( it = msg.items().begin();  it != msg.items().end();  ++it )  {
#ifdef ELfwkJobReportTRACE_LOG
          std::cerr << "      =:=:=: Item:  " << *it << '\n';
#endif
          //  emit( "      <Item> " );
          emit( *it);
          emit( "\n" );
          //emit( " </Item>\n" );
        }
      }

      // Close the body of the message
      //emit ("    </Message>\n");

      // Provide further identification: Module
      //
      //emit ("    <Module> ");
      //emit ( xid.module );
      //emit (" </Module>\n");

#ifdef ELfwkJobReportTRACE_LOG
      std::cerr << "    =:=:=: Module done \n";
#endif

      // close report
      //
      //emit ("  </Report>\n\n");

#ifdef ELfwkJobReportTRACE_LOG
      std::cerr << "  =:=:=: log(msg) done: \n";
#endif

      msg.setReactedTo( true );

    }  // log()

    void ELfwkJobReport::finish()   {
      // closing xml tag
      *osh << "</FrameworkJobReport>\n";
    }

    // Remainder are from base class.


    // ----------------------------------------------------------------------
    // Output methods:
    // ----------------------------------------------------------------------

    void ELfwkJobReport::emit( const ELstring & s, bool /*nl*/ )  {

#ifdef ELfwkJobReport_EMIT_TRACE
      std::cerr << "[][][] in emit:  charsOnLine is " << charsOnLine << '\n';
      std::cerr << "[][][] in emit:  s.length() " << s.length() << '\n';
      std::cerr << "[][][] in emit:  lineLength is " << lineLength << '\n';
#endif

      if (s.length() == 0)  {
        return;
      }

#ifdef ELfwkJobReport_EMIT_TRACE
      std::cerr << "[][][] in emit: about to << s to *osh: " << s << " \n";
#endif

      *osh << s;

#ifdef ELfwkJobReport_EMIT_TRACE
      std::cerr << "[][][] in emit: completed \n";
#endif

    }  // emit()


    // ----------------------------------------------------------------------
    // Summary output:
    // ----------------------------------------------------------------------

    void ELfwkJobReport::summarization( const ELstring & fullTitle,
                                        const ELstring & sumLines ) {
      const int titleMaxLength( 40 );

      // title:
      //
      ELstring title( fullTitle, 0, titleMaxLength );
      int q = (lineLength - title.length() - 2) / 2;
      ELstring line(q, '=');
      emit( "", true );
      emit( line );
      emit( " " );
      emit( title );
      emit( " " );
      emit( line, true );

      // body:
      //
      *osh << sumLines;

      // finish:
      //
      emit( "", true );
      emit( ELstring(lineLength, '='), true );

    }  // summarization()


    // ----------------------------------------------------------------------
    // Changing ostream:
    // ----------------------------------------------------------------------

    void ELfwkJobReport::changeFile (std::ostream & os_) {
      osh = std::make_unique<cet::ostream_observer>(os_);
      emit( "\n=======================================================", true );
      emit( "\nError Log changed to this stream\n" );
      emit( "\n=======================================================\n", true );
    }

    void ELfwkJobReport::changeFile (const ELstring & filename) {
      osh = std::make_unique<cet::ostream_owner>(filename, std::ios/*_base*/::app);
      emit( "\n=======================================================", true );
      emit( "\nError Log changed to this file\n" );
      emit( "\n=======================================================\n", true );
    }

    void ELfwkJobReport::flush()  {
      osh->stream().flush();
    }


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf
