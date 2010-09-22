// ----------------------------------------------------------------------
//
// MessageLoggerScribe.cc
//
// Changes:
//
//   1 - 3/22/06  mf  - in configure_dest()	
//	Repaired the fact that destination limits for categories
//	were not being effective:
//	a) use values from the destination specific default PSet
//	   rather than the overall default PSet to set these
//	b) when an explicit value has been set - either by overall default or 
//	   by a destination specific default PSet - set that limit or
//	   timespan for that dest_ctrl via a "*" msgId.
// 
//   2 - 3/22/06  mf  - in configure_dest()	
//	Enabled the use of -1 in the .cfg file to mean infinite limit
//	or timespan.  This is done by:
//	a) replacing the default value of -1 (by which we recognize 
//	never-specified values) by NO_VALUE_SET = -45654
//	b) checking for values of -1 and substituting a very large integer  
//
//   3 - 4/28/06  mf  - in configure_dest()
//	Mods to help deal with the fact that checking for an empty PSet is
//	unwise when untracked parameters are involved:  The PSet will appear
//	to be empty and if skipped, will result in limits not being applied.
//	a) Replaced default values directly in getAparameter with variables
//	which can be examined all in one place.
//	b) Carefully checked that we are never comparing to the empty PSet
//	
//   4 - 4/28/06  mf  - in configure_dest()
//	If a destination name does not have an extension, append .log 
//	(or in the case of a FwkJobReport, .xml).
//	[note for this change - the filename kept as an index to stream_ps
//	can be kept as its original name; it is just a tool for assigning
//	the right shared stream to statistics destinations]
//
//   5 - 4/28/06  mf  - in configure_dest()
//	Provision for an overall default affecting all categories, for 
//	example, establishing a limit for all a specific category for
//	every destination. 
//
//   6 - 5/18/06 mf  - in configure_dest()
//	Implement establishing intervals between reacting to message of
//	some type.
//
//   7 - 5/24/06 mf  - in configure_dest()
//	Corrected algorithm for estabolishing limits and intervals, avoiding
//      interference between setting the one and getting the default for the 
//      other.
//
//   8 - 5/31/06 wmtan  - in configure_errorlog()
//	The presence of the framework job report should not affect the output
//      to the early destination (cerr).
//
//   9 - 6/6/06 mf  - in configure_dest()
//	Support for placeholder PSet without actually creating the destination.
//	Useful in a .cfi file, in conjunction with potential replace commands.
//
//  10 - 6/6/06 mf  - in configure_dest()
//	Changed cfg keyword interval to reportEvery
//
//  11 - 6/12/06 mf  - in configure_errorlog()
//	Check for placeholder before attaching a destination that may not be
//	wanted.
//
//  12 - 6/14/06 mf  - in configure_external_dests()
//	Clear the list of external dests needing configuration, if there
//	is no configuration file available.
//
//  13 - 8/7/06 mf  - in configure_external_dests()
//	Undo change 12: the list of external dests needing configuration
//      is left intact if there is no configuration file available, the
//      assumption being that at some later time there will be a file and
//      the message logger will be configured again.
//
//      Note: The change made in (12) and un-done here was necessary to
//            prevent segfault behavior when a job is done with external
//            destinations and no .cfg file under some circumstances.
//            D. Evans (who was being hit with that behavior due to an
//	      accidental .cfg omission) asserts (8/16) that running with
//            no .cfg file is a sufficient anomoly that the current change
//            is acceptable. 
//
//  14 - 10/18/06 mf  - in configure_error_log()
//	Finer control of output file name for a given destination:
//	Accept a parameter extension, to specify some extension other than
//      .log without needing to place a dot in the Pset name.  Also accept
//	an explicit filename.
//
//  15 - 2/11/07 mf - at bottom
//	Declared static_errorlog_p
//
//  16 - 3/13/07 mf - in configure_errorlog() and addition of 3 functions
//	 Break out the configuring of each type of destination, for sanity.
//
//  17 - 3/13/07 mf - in run(), at CONFIGURE case
//	 Use the handshake to make this synchronous, and pass any throw
//	 across to the other thread.
//
//  18 - 3/14/07 mf - in configure_ordinary_destinations() and 2 others
//	 Use actual filename in a master ostream_ps list, and behave correctly
//	 when duplicates are found (duplicate names both used leads to grim
//	 file behavior when one file is opened as two streams).
//
//  19 - 3/15/07 mf - in configure_fwkJobReports()
//	 "Deturdification" - default is to not produce a job reports; a
//	 command-line option lets you produce them.
//
//  20 - 3/15/07 mf - in configure_statistics() and configure_fwkJobReports()
//	 Handle the placeholder case 
//
//  21 - 3/15/07 mf - run()
//	 Improve the behavior of catches of exceptions in non-synchronous 
//       command cases:  Unless the ConfigurationHandshake is used, re-throw
//	 is not an option, but exit is also not very good.
//
//  22 - 4/18/07 mf - in configure_error_log and its called functions
//	 Allow for duplicate file names if configuration happens twice.
//
//  23 - 6/13/07 mf - in configure_statistics 
//	 Repared error of calling "cerr" "err", which would cause appended
//	 statistics destinations going to cerr to instead come out in a file 
//	 err.log
//
//  24 - 6/15/07 mf - in configure_errlog and its descendants
//	 Major mods to hardwire defaults taken from the .cfi file 
//	 To allow flexibility, this depends on MessageLoggerDefaults.h
//
//  25 - 7/24/07 mf - in run() 
//	 A command SHUT_UP to deactivate, and in the LOG_A_MESSGE case, response to 
//	 that command.  This allows supression of the generator info in case of a
//	 completely .cfg-less cetRun command.  
//
//  26 - 8/7/07 mf - in run()
//	 A command FLUSH_LOG_Q to consume the entire queue, processing each 
//	 message.  Actually, the key is that on the other side, it is used in
//       a synchronous manner (like CONFIGURE) so as soon as one gets up to 
//	 the flush command, the queue has in fact been flushed!  
//
//  27 - 8/16/07 mf - in run()
//	 A command GROUP_STATS to add a category to a list which ELstatistics 
//	 will use to avoid separate per-module statistics for that category.
//
//  28 - 6/18/08 mf - in CONFIGURE case and FLUSH_LOG_Q case
//	 Changed expectation of p from a ParameterSet* to a void*:
//	 static cast it to the needed ParameterSet*
//
//  29 - 6/19/08 mf - in run() and a new function triggerFJRmessageSummary()
//	 Implemented filling a map with summary info for the Job Report
//
//  30 - 6/20/08 mf - in run() 
//	 Setting MessageLoggerScribeIsRunning
//
//  31 - 7/9/08  mf - in configure_ordinary_destinations()
//		 and configure_statistics()
//	 using hardwired default output filename if there is one
//
//  32 - 10/21/08 mf - in ctor and in run() and new runCommand()
//	 split up run() to have ability to implement single-thread
//
//  33 - 10/22/08 mf
//	 implementation of singleThread
//
//  34 - 5/13/09 mf
//	 Allowing threshold to be set for default destination in default PSet
//
//  35 - 5/29/09 mf
//       Avoiding throw when duplicate destination names are used, to let the
//	 validation report that and abort instead.
//
//  35 - 8/10/09 mf, cdj
//       Use ThreadQ in place of the singleton MessageLoggerQ to consume
//
// ----------------------------------------------------------------------

#include "MessageService/interface/MessageLoggerScribe.h"
#include "MessageService/interface/ELadministrator.h"
#include "MessageService/interface/ELoutput.h"
#include "MessageService/interface/ELstatistics.h"
#include "MessageService/interface/ELfwkJobReport.h"
#include "MessageService/interface/ErrorLog.h"
#include "MessageService/interface/ThreadQueue.h"

#include "MessageLogger/interface/ErrorObj.h"
#include "MessageLogger/interface/MessageLoggerQ.h"
#include "MessageLogger/interface/MessageLogger.h"
#include "MessageLogger/interface/ConfigurationHandshake.h"

#include "Utilities/interface/EDMException.h"
#include "Utilities/interface/Algorithms.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <signal.h>

#include <dlfcn.h> //dlopen

using std::cerr;

namespace mf {
namespace service {


MessageLoggerScribe::MessageLoggerScribe(boost::shared_ptr<ThreadQueue> queue)
: admin_p   ( ELadministrator::instance() )
, early_dest( admin_p->attach(ELoutput(std::cerr, false)) )
, errorlog_p( new ErrorLog() )
, file_ps   ( )
, job_pset_p( )
, extern_dests( )
, jobReportOption( )
, clean_slate_configuration( true )
, active( true )
, singleThread (queue.get() == 0)				// changeLog 36
, done (false)							// changeLog 32
, purge_mode (false)						// changeLog 32
, count (false)							// changeLog 32
, m_queue(queue)						// changeLog 36
{
  admin_p->setContextSupplier(msg_context);
}

MessageLoggerScribe::~MessageLoggerScribe()
{
  admin_p->finish();
  assert( extern_dests.empty() );  // nothing to do
}


void
  MessageLoggerScribe::run()
{
  MessageLoggerQ::OpCode  opcode;
  void *                  operand;

  MessageDrop::instance()->messageLoggerScribeIsRunning = 
  				MLSCRIBE_RUNNING_INDICATOR;	// ChangeLog 30
//  std::cerr << "MessageLoggerScribe::run(): \n";
//  std::cerr << "messageLoggerScribeIsRunning = "
//      << (int)MessageDrop::instance()->messageLoggerScribeIsRunning << "\n";

  do  {
    m_queue->consume(opcode, operand);  // grab next work item from Q 
    					// changeLog 36
    runCommand (opcode, operand);
  } while(! done);

}  // MessageLoggerScribe::run()

void
  MessageLoggerScribe::runCommand(				// changeLog 32
  	MessageLoggerQ::OpCode  opcode, 
	void * operand)
{
  switch(opcode)  {  // interpret the work item
    default:  {
      assert(false);  // can't happen (we certainly hope!)
      break;
    }
    case MessageLoggerQ::END_THREAD:  {
      assert( operand == 0 );
      done = true;
      MessageDrop::instance()->messageLoggerScribeIsRunning = 
	      (unsigned char) -1; 				// ChangeLog 30
      break;
    }
    case MessageLoggerQ::LOG_A_MESSAGE:  {
      ErrorObj *  errorobj_p = static_cast<ErrorObj *>(operand);
      try {
	if(active && !purge_mode) log (errorobj_p);        
      }
      catch(cet::Exception& e)
      {
	  ++count;
	  std::cerr << "MessageLoggerScribe caught " << count
	       << " cet::Exceptions, text = \n"
	       << e.what() << "\n";

	  if(count > 25)
	    {
	      cerr << "MessageLogger will no longer be processing "
		   << "messages due to errors (entering purge mode).\n";
	      purge_mode = true;
	    }
      }
      catch(...)
      {
	  std::cerr << "MessageLoggerScribe caught an unknown exception and "
	       << "will no longer be processing "
	       << "messages. (entering purge mode)\n";
	  purge_mode = true;
      }
      delete errorobj_p;  // dispose of the message text
      break;
    }
    case MessageLoggerQ::CONFIGURE:  {			// changelog 17
      if (singleThread) {
	job_pset_p.reset(static_cast<fhicl::ParameterSet *>(operand));
	configure_errorlog();
	break;
      } else {
	ConfigurationHandshake * h_p = 
		static_cast<ConfigurationHandshake *>(operand);
	job_pset_p.reset(static_cast<fhicl::ParameterSet *>(h_p->p));
	boost::mutex::scoped_lock sl(h_p->m);   // get lock
	try {
	  configure_errorlog();
	}
	catch(mf::Exception& e)
	  {
	    Place_for_passing_exception_ptr epp = h_p->epp;
	    if (!(*epp)) { 
	      *epp = boost::shared_ptr<mf::Exception>(new mf::Exception(e));
	    } else {
	      Pointer_to_new_exception_on_heap ep = *epp;
	      (*ep) << "\n and another exception: \n" << e.what();
	    }
	  }
	// Note - since the configuring code has not made a new copy of the 
	// job parameter set, we must not delete job_pset_p (in contrast to
	// the case for errorobj_p).  On the other hand, if we instantiate
	// a new mf::Exception pointed to by *epp, it is the responsibility
	// of the MessageLoggerQ to delete it.
	h_p->c.notify_all();  // Signal to MessageLoggerQ that we are done
	// finally, release the scoped lock by letting it go out of scope 
	break;
      }
    }
    case MessageLoggerQ::EXTERN_DEST: {
      try {
	extern_dests.push_back( static_cast<NamedDestination *>(operand) );
	configure_external_dests();
      }
      catch(cet::Exception& e)				// change log 21
	{
	  std::cerr << "MessageLoggerScribe caught a cet::Exception "
	       << "during extern dest configuration:\n"
	       << e.what() << "\n"
	       << "This is a serious problem, and the extern dest " 
	       << "will not be produced.\n"
	       << "However, the rest of the logger continues to run.\n";
	}
      catch(...)						// change log 21
	{
	  std::cerr << "MessageLoggerScribe caught unkonwn exception type\n"
	       << "during extern dest configuration. "
	       << "This is a serious problem, and the extern dest " 
	       << "will not be produced.\n"
	       << "The rest of the logger will attempt to continue to run.\n";
	}
      break;
    }
    case MessageLoggerQ::SUMMARIZE: {
      assert( operand == 0 );
      try {
	triggerStatisticsSummaries();
      }
      catch(cet::Exception& e)
	{
	  std::cerr << "MessageLoggerScribe caught exception "
	       << "during summarize:\n"
	       << e.what() << "\n";
	}
      catch(...)
	{
	  std::cerr << "MessageLoggerScribe caught unkonwn exception type "
	       << "during summarize. (Ignored)\n";
	}
      break;
    }
    case MessageLoggerQ::JOBREPORT:  {			// change log 19
      std::string* jobReportOption_p =
	      static_cast<std::string*>(operand);
      try {
	jobReportOption = *jobReportOption_p;
      }
      catch(cet::Exception& e)
	{
	  std::cerr << "MessageLoggerScribe caught a cet::Exception "
	       << "during processing of --jobReport option:\n"
	       << e.what() << "\n"
	       << "This likely will affect or prevent the job report.\n"
	       << "However, the rest of the logger continues to run.\n";
	}
      catch(...)
	{
	  std::cerr << "MessageLoggerScribe caught unkonwn exception type\n"
	       << "during processing of --jobReport option.\n"
	       << "This likely will affect or prevent the job report.\n"
	       << "However, the rest of the logger continues to run.\n";
	}
      delete jobReportOption_p;  // dispose of the message text
				 // which will have been new-ed
				 // in MessageLogger.cc (service version)
      break;
    }
    case MessageLoggerQ::JOBMODE:  {			// change log 24
      std::string* jobMode_p =
	      static_cast<std::string*>(operand);
      JobMode jm = MessageLoggerDefaults::mode(*jobMode_p);
      messageLoggerDefaults = 
	value_ptr<MessageLoggerDefaults>(new MessageLoggerDefaults(jm));
	      // Note - since messageLoggerDefaults is a value_ptr, 
	      //        there is no concern about deleting here.
      delete jobMode_p;  // dispose of the message text
      			 // which will have been new-ed
			 // in MessageLogger.cc (service version)
      break;
    }
    case MessageLoggerQ::SHUT_UP:  {
      assert( operand == 0 );
      active = false;
      break;
    }
    case MessageLoggerQ::FLUSH_LOG_Q:  {			// changelog 26
      if (singleThread) return;
      ConfigurationHandshake * h_p = 
	      static_cast<ConfigurationHandshake *>(operand);
      job_pset_p.reset(static_cast<fhicl::ParameterSet *>(h_p->p));
      boost::mutex::scoped_lock sl(h_p->m);   // get lock
      h_p->c.notify_all();  // Signal to MessageLoggerQ that we are done
      // finally, release the scoped lock by letting it go out of scope 
      break;
    }
    case MessageLoggerQ::GROUP_STATS:  {			// change log 27
      std::string* cat_p =
	      static_cast<std::string*>(operand);
      ELstatistics::noteGroupedCategory(*cat_p);
      delete cat_p;  // dispose of the message text
      break;
    }
    case MessageLoggerQ::FJR_SUMMARY:  {			// changelog 29
      if (singleThread) {
	std::map<std::string, double> * smp = 
		static_cast<std::map<std::string, double> *>(operand);
	triggerFJRmessageSummary(*smp);
	break;
      } else {
	ConfigurationHandshake * h_p = 
		static_cast<ConfigurationHandshake *>(operand);
	boost::mutex::scoped_lock sl(h_p->m);   // get lock
	std::map<std::string, double> * smp = 
		static_cast<std::map<std::string, double> *>(h_p->p);
	triggerFJRmessageSummary(*smp);
	h_p->c.notify_all();  // Signal to MessageLoggerQ that we are done
	// finally, release the scoped lock by letting it go out of scope 
	break;
      }
    }
    case MessageLoggerQ::SWITCH_CHANNEL:  {
      std::string * chanl_p = 
              static_cast<std::string*>(operand);
      errorlog_p -> switchChannel( *chanl_p );
      delete chanl_p;
      break;
    }
  }  // switch

}  // MessageLoggerScribe::runCommand(opcode, operand)

void MessageLoggerScribe::log ( ErrorObj *  errorobj_p ) {
  ELcontextSupplier& cs =
    const_cast<ELcontextSupplier&>(admin_p->getContextSupplier());
  MsgContext& mc = dynamic_cast<MsgContext&>(cs);
  mc.setContext(errorobj_p->context());
  std::vector<std::string> categories;
  parseCategories(errorobj_p->xid().id, categories);
  for (unsigned int icat = 0; icat < categories.size(); ++icat) {
    errorobj_p->setID(categories[icat]);
    (*errorlog_p)( *errorobj_p );  // route the message text
  }
}

void
  MessageLoggerScribe::configure_errorlog()
{
  vString  empty_vString;
  String   empty_String;
  PSet     empty_PSet;
  
  // The following is present to test pre-configuration message handling:
  String preconfiguration_message 
       = job_pset_p->getString
       	("generate_preconfiguration_message", empty_String);
  if (preconfiguration_message != empty_String) {
    // To test a preconfiguration message without first going thru the 
    // configuration we are about to do, we issue the message (so it sits
    // on the queue), then copy the processing that the LOG_A_MESSAGE case
    // does.  We suppress the timestamp to allow for automated unit testing.
    early_dest.suppressTime();
    LogError ("preconfiguration") << preconfiguration_message;
    if (!singleThread) {
      MessageLoggerQ::OpCode  opcode;
      void *                  operand;
      m_queue->consume(opcode, operand);  // grab next work item from Q
      assert (opcode == MessageLoggerQ::LOG_A_MESSAGE);
      ErrorObj *  errorobj_p = static_cast<ErrorObj *>(operand);
      log (errorobj_p);        
      delete errorobj_p;  // dispose of the message text
    }
  }

  if ( !stream_ps.empty() ) {
    LogWarning ("multiLogConfig") 
    	<< "The message logger has been configured multiple times"; 
    clean_slate_configuration = false;				// Change Log 22
  }

  void *hndl = dlopen("libMF_ExtModules.so", RTLD_NOW );
  if(hndl == NULL) {
    LogError("preconfiguration") << dlerror();
  }  

  configure_fwkJobReports();					// Change Log 16
  configure_ordinary_destinations();				// Change Log 16
  configure_statistics();					// Change Log 16

  configure_external_dests();

}  // MessageLoggerScribe::configure_errorlog()




void
  MessageLoggerScribe::configure_dest( ELdestControl & dest_ctrl
                                     , String const & dest_pset_name 
                                     , fhicl::ParameterSet const & dest_pset
				     )
{
  static const int NO_VALUE_SET = -45654;			// change log 2
  vString  empty_vString;
  PSet     empty_PSet;
  String   empty_String;

  // Defaults:							// change log 3a
  const std::string COMMON_DEFAULT_THRESHOLD = "INFO";
  const         int COMMON_DEFAULT_LIMIT     = NO_VALUE_SET; 
  const         int COMMON_DEFAULT_INTERVAL  = NO_VALUE_SET; 	// change log 6
  const         int COMMON_DEFAULT_TIMESPAN  = NO_VALUE_SET; 

  char const*  severity_array[] = {"WARNING", "INFO", "ERROR", "DEBUG"};
  vString const  severities(severity_array+0, severity_array+4);

  // grab the pset for category list for this destination
  PSet cats_pset = dest_pset.getPSet("categories");

  // grab list of categories
  vString  categories = cats_pset.getPSetNameList();
  vString::iterator it = categories.begin();
  while(it!=categories.end()) { 
    if(*it == "default") it=categories.erase(it); 
    else ++it;
  }

  // grab list of hardwired categories (hardcats) -- these are to be added
  // to the list of categories -- change log 24
  {
    std::vector<std::string> hardcats = messageLoggerDefaults->categories;
  // combine the lists, not caring about possible duplicates (for now)
    copy_all( hardcats, std::back_inserter(categories) );
  }  // no longer need hardcats

  // See if this is just a placeholder			// change log 9
  bool is_placeholder 
      = dest_pset.getBool("placeholder", false);
  if (is_placeholder) return;
  
  // default threshold for the destination
  String default_threshold = empty_String;

  // grab this destination's default limit/interval/timespan:
  PSet  category_default_pset = cats_pset.getPSet("default", empty_PSet);
  
  int  dest_default_limit 
         = category_default_pset.getInt("limit", COMMON_DEFAULT_LIMIT);
  int  dest_default_interval 
         = category_default_pset.getInt("reportEvery", COMMON_DEFAULT_INTERVAL);
  int  dest_default_timespan 
         = category_default_pset.getInt("timespan", COMMON_DEFAULT_TIMESPAN);

  if ( dest_default_limit != NO_VALUE_SET ) 
  {
    if ( dest_default_limit < 0 ) dest_default_limit = 2000000000;
    dest_ctrl.setLimit("*", dest_default_limit );
  } 						// change log 1b, 2a, 2b
  if ( dest_default_interval != NO_VALUE_SET ) 
  {  // change log 6
    dest_ctrl.setInterval("*", dest_default_interval );
  } 						
  if ( dest_default_timespan != NO_VALUE_SET ) 
  {
    if ( dest_default_timespan < 0 ) dest_default_timespan = 2000000000;
    dest_ctrl.setTimespan("*", dest_default_timespan );
  } 						// change log 1b, 2a, 2b
    						  
  // establish this destination's threshold:
  String dest_threshold = dest_pset.getString("threshold", default_threshold);

  if (dest_threshold == empty_String) {  
    dest_threshold = default_threshold;
  }
  if (dest_threshold == empty_String) {
    dest_threshold = messageLoggerDefaults->threshold(dest_pset_name);
  }
  if (dest_threshold == empty_String) {
    dest_threshold = COMMON_DEFAULT_THRESHOLD;
  }

  ELseverityLevel  threshold_sev(dest_threshold);
  dest_ctrl.setThreshold(threshold_sev);

  // establish this destination's limit/interval/timespan for each category:
  PSet default_category_pset = cats_pset.getPSet("default");

  for( vString::const_iterator id_it = categories.begin()
     ; id_it != categories.end()
     ; ++id_it
     )
  {
    String  msgID = *id_it;

    PSet category_pset
       = cats_pset.getPSet(msgID, default_category_pset);

    int  category_default_limit 
       = default_category_pset.getInt("limit", NO_VALUE_SET);
    int  limit
       = category_pset.getInt("limit", category_default_limit);
    if (limit == NO_VALUE_SET) limit = dest_default_limit;
       								// change log 7 
    int  category_default_interval 
       = default_category_pset.getInt("reportEvery", NO_VALUE_SET);
    int  interval
       = category_pset.getInt("reportEvery",category_default_interval);
    if (interval == NO_VALUE_SET) interval = dest_default_interval;
      						// change log 6  and then 7
    int  category_default_timespan 
       = default_category_pset.getInt("timespan", NO_VALUE_SET);
    int  timespan
       = category_pset.getInt("timespan", category_default_timespan);
    if (timespan == NO_VALUE_SET) timespan = dest_default_timespan;
       								// change log 7 

    std::string category = msgID;
    if ( limit     == NO_VALUE_SET )  {				// change log 24
       limit = messageLoggerDefaults->limit(dest_pset_name,category);
    }  
    if ( interval     == NO_VALUE_SET )  {			// change log 24
       interval = messageLoggerDefaults->reportEvery(dest_pset_name,category);
    }  
    if ( timespan     == NO_VALUE_SET )  {			// change log 24
       timespan = messageLoggerDefaults->timespan(dest_pset_name,category);
    }  
     
    if( limit     != NO_VALUE_SET )  {
      if ( limit < 0 ) limit = 2000000000;  
      dest_ctrl.setLimit(msgID, limit);
    }  						// change log 2a, 2b
    if( interval  != NO_VALUE_SET )  {
      dest_ctrl.setInterval(msgID, interval);
    }  						// change log 6
    if( timespan  != NO_VALUE_SET )  {
      if ( timespan < 0 ) timespan = 2000000000;  
      dest_ctrl.setTimespan(msgID, timespan);
    }						// change log 2a, 2b
						
  }  // for

  // establish this destination's linebreak policy:
  bool noLineBreaks 
  	= dest_pset.getBool ("noLineBreaks", false); 
  if (noLineBreaks) {
    dest_ctrl.setLineLength(32000);
  }
  else {
    int  lenDef = 80;
    int  lineLen = dest_pset.getInt ("lineLength", lenDef);
    if (lineLen != lenDef) {
      dest_ctrl.setLineLength(lineLen);
    }
  }

  // if indicated, suppress time stamps in this destination's output
  bool suppressTime 
  	= dest_pset.getBool ("noTimeStamps", false);
  if (suppressTime) {
    dest_ctrl.suppressTime();
  }

  // enable or disable milliseconds in the timestamp
  bool millisecondTimestamp
        = dest_pset.getBool ("useMilliseconds", false);
  if (millisecondTimestamp) {
    dest_ctrl.includeMillisecond();
  }

#if 0
  // grab list of categories
  vString  categories = job_pset_p->getVString("categories", empty_vString);

  // grab list of messageIDs -- these are a synonym for categories
  // Note -- the use of messageIDs is deprecated in favor of categories
  {
    vString  messageIDs = job_pset_p->getVString("messageIDs", empty_vString);

  // combine the lists, not caring about possible duplicates (for now)
    copy_all( messageIDs, std::back_inserter(categories) );
  }  // no longer need messageIDs

  // grab list of hardwired categories (hardcats) -- these are to be added
  // to the list of categories -- change log 24
  {
    std::vector<std::string> hardcats = messageLoggerDefaults->categories;
  // combine the lists, not caring about possible duplicates (for now)
    copy_all( hardcats, std::back_inserter(categories) );
  }  // no longer need hardcats

  // grab default threshold common to all destinations
  String default_threshold = job_pset_p->getString("threshold", empty_String);
     						// change log 3a
						// change log 24

  // grab default limit/interval/timespan common to all destinations/categories:
  PSet  default_pset
     = job_pset_p->getPSet("default", empty_PSet);
  int  default_limit
    = default_pset.getInt("limit", COMMON_DEFAULT_LIMIT);
  int  default_interval
    = default_pset.getInt("reportEvery", COMMON_DEFAULT_INTERVAL);
    						// change log 6, 10
  int  default_timespan
    = default_pset.getInt("timespan", COMMON_DEFAULT_TIMESPAN);
						// change log 2a
    						// change log 3a
  String default_pset_threshold				
     = default_pset.getString("threshold", default_threshold);
     						// change log 34
			
  // grab all of this destination's parameters:
  PSet  dest_pset = job_pset_p->getPSet(filename, empty_PSet);

  // See if this is just a placeholder			// change log 9
  bool is_placeholder 
      = dest_pset.getBool("placeholder", false);
  if (is_placeholder) return;
  
  // grab this destination's default limit/interval/timespan:
  PSet  dest_default_pset
     = dest_pset.getPSet("default", empty_PSet);
  int  dest_default_limit
    = dest_default_pset.getInt("limit", default_limit);
  int  dest_default_interval
    = dest_default_pset.getInt("reportEvery", default_interval);
    						// change log 6
  int  dest_default_timespan
    = dest_default_pset.getInt("timespan", default_timespan);
    						// change log 1a
  if ( dest_default_limit != NO_VALUE_SET ) {
    if ( dest_default_limit < 0 ) dest_default_limit = 2000000000;
    dest_ctrl.setLimit("*", dest_default_limit );
  } 						// change log 1b, 2a, 2b
  if ( dest_default_interval != NO_VALUE_SET ) {  // change log 6
    dest_ctrl.setInterval("*", dest_default_interval );
  } 						
  if ( dest_default_timespan != NO_VALUE_SET ) {
    if ( dest_default_timespan < 0 ) dest_default_timespan = 2000000000;
    dest_ctrl.setTimespan("*", dest_default_timespan );
  } 						// change log 1b, 2a, 2b
    						  
  // establish this destination's threshold:
  String dest_threshold
     = dest_pset.getString("threshold", default_threshold);
  if (dest_threshold == empty_String) {  
    dest_threshold = default_threshold;
  }
  if (dest_threshold == empty_String) {			// change log 34
      dest_threshold = default_pset_threshold;
  }
  if (dest_threshold == empty_String) {
    dest_threshold = messageLoggerDefaults->threshold(filename);
  }
  if (dest_threshold == empty_String) dest_threshold = COMMON_DEFAULT_THRESHOLD;
  ELseverityLevel  threshold_sev(dest_threshold);
  dest_ctrl.setThreshold(threshold_sev);

  // establish this destination's limit/interval/timespan for each category:
  for( vString::const_iterator id_it = categories.begin()
     ; id_it != categories.end()
     ; ++id_it
     )
  {
    String  msgID = *id_it;
    PSet default_category_pset 
       = default_pset.getPSet(msgID, empty_PSet);	// change log 5
    PSet  category_pset
       = dest_pset.getPSet(msgID, default_category_pset);

    int  category_default_limit 
       = default_category_pset.getInt("limit", NO_VALUE_SET);
    int  limit
      = category_pset.getInt("limit", category_default_limit);
    if (limit == NO_VALUE_SET) limit = dest_default_limit;
       								// change log 7 
    int  category_default_interval 
       = default_category_pset.getInt("reportEvery", NO_VALUE_SET);
    int  interval
      = category_pset.getInt("reportEvery",category_default_interval);
    if (interval == NO_VALUE_SET) interval = dest_default_interval;
      						// change log 6  and then 7
    int  category_default_timespan 
       = default_category_pset.getInt("timespan", NO_VALUE_SET);
    int  timespan
      = category_pset.getInt("timespan", category_default_timespan);
    if (timespan == NO_VALUE_SET) timespan = dest_default_timespan;
       								// change log 7 

    std::string category = msgID;
    if ( limit     == NO_VALUE_SET )  {				// change log 24
       limit = messageLoggerDefaults->limit(filename,category);
    }  
    if ( interval     == NO_VALUE_SET )  {			// change log 24
       interval = messageLoggerDefaults->reportEvery(filename,category);
    }  
    if ( timespan     == NO_VALUE_SET )  {			// change log 24
       timespan = messageLoggerDefaults->timespan(filename,category);
    }  
     
    if( limit     != NO_VALUE_SET )  {
      if ( limit < 0 ) limit = 2000000000;  
      dest_ctrl.setLimit(msgID, limit);
    }  						// change log 2a, 2b
    if( interval  != NO_VALUE_SET )  {
      dest_ctrl.setInterval(msgID, interval);
    }  						// change log 6
    if( timespan  != NO_VALUE_SET )  {
      if ( timespan < 0 ) timespan = 2000000000;  
      dest_ctrl.setTimespan(msgID, timespan);
    }						// change log 2a, 2b
						
  }  // for

  // establish this destination's limit for each severity:
  for( vString::const_iterator sev_it = severities.begin()
     ; sev_it != severities.end()
     ; ++sev_it
     )
  {
    String  sevID = *sev_it;
    ELseverityLevel  severity(sevID);
    PSet  default_sev_pset 
    	= default_pset.getPSet(sevID, empty_PSet);
    PSet  sev_pset 
    	= dest_pset.getPSet(sevID, default_sev_pset);
						// change log 5
    int  limit     = sev_pset.getInt("limit", NO_VALUE_SET);
    if ( limit     == NO_VALUE_SET )  {				// change log 24
       limit = messageLoggerDefaults->sev_limit(filename,sevID);
    }  
    if( limit    != NO_VALUE_SET )  dest_ctrl.setLimit(severity, limit   );
    int  interval  = sev_pset.getInt("reportEvery", NO_VALUE_SET);
    if ( interval     == NO_VALUE_SET )  {			// change log 24
       interval = messageLoggerDefaults->sev_reportEvery(filename,sevID);
    }  
    if( interval != NO_VALUE_SET )  dest_ctrl.setInterval(severity, interval);
						// change log 2
    int  timespan  = sev_pset.getInt("timespan", NO_VALUE_SET);
    if ( timespan     == NO_VALUE_SET )  {			// change log 24
       timespan = messageLoggerDefaults->sev_timespan(filename,sevID);
    }  
    if( timespan != NO_VALUE_SET )  dest_ctrl.setTimespan(severity, timespan   );
    						// change log 6
  }  // for

  // establish this destination's linebreak policy:
  bool noLineBreaks_default 
  	= default_pset.getBool ("noLineBreaks", false);
						// change log 5
  bool noLineBreaks 
  	= dest_pset.getBool ("noLineBreaks", noLineBreaks_default);
  if (noLineBreaks) {
    dest_ctrl.setLineLength(32000);
  }
  else {
    int  lenDef = 80;
    int  lineLen_default
    	 = default_pset.getInt ("lineLength", lenDef);
						// change log 5
    int  lineLen = dest_pset.getInt ("lineLength", lineLen_default);
    if (lineLen != lenDef) {
      dest_ctrl.setLineLength(lineLen);
    }
  }

  // if indicated, suppress time stamps in this destination's output
  bool suppressTime_default 
  	= default_pset.getBool ("noTimeStamps", false);
  bool suppressTime 
  	= dest_pset.getBool ("noTimeStamps", suppressTime_default);
  if (suppressTime) {
    dest_ctrl.suppressTime();
  }

  // enable to disable millisecond in the timestamp
  bool millisecondTimestamp_default
        = default_pset.getBool ("useMilliseconds", false);
  bool millisecondTimestamp
        = dest_pset.getBool ("useMilliseconds", millisecondTimestamp_default);
  if (millisecondTimestamp) {
    dest_ctrl.includeMillisecond();
  }
#endif
}  // MessageLoggerScribe::configure_dest()

void
  MessageLoggerScribe::configure_default_fwkJobReport 
  				( ELdestControl & dest_ctrl ) 
{
 
  dest_ctrl.setLimit("*", 0 );
  String  msgID = "FwkJob";
  int FwkJob_limit = 10000000;
  dest_ctrl.setLimit(msgID, FwkJob_limit);
  dest_ctrl.setLineLength(32000);
  dest_ctrl.suppressTime();
 
}  // MessageLoggerScribe::configure_default_fwkJobReport()


void
  MessageLoggerScribe::configure_fwkJobReports()		// Changelog 16
{
  vString  empty_vString;
  String   empty_String;
  PSet     empty_PSet;
  
  // decide whether to configure any job reports at all		// Changelog 19
  bool jobReportExists  = false;
  bool enableJobReports = false;
  #ifdef DEFINE_THIS_TO_MAKE_REPORTS_THE_DEFAULT
  enableJobReports = true;
  #endif
  if (jobReportOption != empty_String) enableJobReports = true;
  if (jobReportOption == "~") enableJobReports = false; //  --nojobReport
  if (!enableJobReports) return;
   
  if ((jobReportOption != "*") && (jobReportOption != empty_String)) {
    const std::string::size_type npos = std::string::npos;
    if ( jobReportOption.find('.') == npos ) {
      jobReportOption += ".xml";
    }  
  }

  // grab list of fwkJobReports:
  vString  fwkJobReports
     = job_pset_p->getVString("fwkJobReports", empty_vString);

  // Use the default list of fwkJobReports if and only if the grabbed list is
  // empty						 	// change log 24
  if (fwkJobReports.empty()) {
    fwkJobReports = messageLoggerDefaults->fwkJobReports;
  }
  
  // establish each fwkJobReports destination:
  for( vString::const_iterator it = fwkJobReports.begin()
     ; it != fwkJobReports.end()
     ; ++it
     )
  {
    String filename = *it;
    String psetname = filename;

    // check that this destination is not just a placeholder // change log 20
    PSet  fjr_pset = job_pset_p->getPSet(psetname, empty_PSet);
    bool is_placeholder 
	= fjr_pset.getBool("placeholder", false);
    if (is_placeholder) continue;

    // Modify the file name if extension or name is explicitly specified
    // change log 14 
    String explicit_filename 
        = fjr_pset.getString("filename", empty_String);
    if (explicit_filename != empty_String) filename = explicit_filename;
    String explicit_extension 
        = fjr_pset.getString("extension", empty_String);
    if (explicit_extension != empty_String) {
      if (explicit_extension[0] == '.') {
	filename += explicit_extension;             
      } else {
	filename = filename + "." + explicit_extension;   
      }
    }

    // Attach a default extension of .xml if there is no extension on a file
    std::string actual_filename = filename;			// change log 4
    const std::string::size_type npos = std::string::npos;
    if ( filename.find('.') == npos ) {
      actual_filename += ".xml";
    }  

     // Check that this is not a duplicate name			// change log 18
    if ( stream_ps.find(actual_filename)!=stream_ps.end() ) {        
      if (clean_slate_configuration) {				// change log 22
       throw mf::Exception ( mf::errors::Configuration ) 
       <<"Duplicate name for a MessageLogger Framework Job Report Destination: " 
       << actual_filename
       << "\n";
      } else {
       LogWarning("duplicateDestination")
       <<"Duplicate name for a MessageLogger Framework Job Report Destination: " 
       << actual_filename
       << "\n" << "Only original configuration instructions are used";
       continue;
      }
     } 
    
    jobReportExists = true;					// Changelog 19
    if ( actual_filename == jobReportOption ) jobReportOption = empty_String;   
    
    boost::shared_ptr<std::ofstream> os_sp(new std::ofstream(actual_filename.c_str()));
    file_ps.push_back(os_sp);
    ELdestControl dest_ctrl;
    dest_ctrl = admin_p->attach( ELfwkJobReport(*os_sp) );
    stream_ps[actual_filename] = os_sp.get();

    // now configure this destination:
    configure_dest(dest_ctrl, psetname, fjr_pset);	

  }  // for [it = fwkJobReports.begin() to end()]

  // Now possibly add the file specified by --jobReport 	// Changelog 19
  if (jobReportOption==empty_String) return;
  if (jobReportExists && ( jobReportOption=="*" )) return;
  if (jobReportOption=="*") jobReportOption = "FrameworkJobReport.xml";
  // Check that this report is not already on order -- here the duplicate
  // name would not be a configuration error, but we shouldn't do it twice			
  std::string actual_filename = jobReportOption;
  if ( stream_ps.find(actual_filename)!=stream_ps.end() ) return;

  boost::shared_ptr<std::ofstream> os_sp(new std::ofstream(actual_filename.c_str()));
  file_ps.push_back(os_sp);
  ELdestControl dest_ctrl;
  dest_ctrl = admin_p->attach( ELfwkJobReport(*os_sp) );
  stream_ps[actual_filename] = os_sp.get();

  // now configure this destination, in the jobreport default manner:
  configure_default_fwkJobReport (dest_ctrl);	

}

void
  MessageLoggerScribe::configure_ordinary_destinations()	// Changelog 16
{
  vString  empty_vString;
  String   empty_String;
  PSet     empty_PSet;

  // grab list of destinations:
  PSet dests = job_pset_p->getPSet("destinations");
  vString  destinations = dests.getPSetNameList();

  // exclude statistic destinations:
  {
    vString::iterator it = destinations.begin();
    while(it!=destinations.end()) { 
      if(*it == "statistics") it=destinations.erase(it); 
      else ++it;
    }
  }

  // Use the default list of destinations if and only if the grabbed list is
  // empty						 	// change log 24
  if (destinations.empty()) {
    destinations = messageLoggerDefaults->destinations;
  }
  
  // dial down the early destination if other dest's are supplied:
  if( ! destinations.empty() )
    early_dest.setThreshold(ELhighestSeverity);

  // establish each destination:
  for( vString::const_iterator it = destinations.begin()
     ; it != destinations.end()
     ; ++it
     )
  {
    String psetname = *it;
    String filename = psetname;
    
    // Retrieve the destination pset object
    PSet  dest_pset = dests.getPSet(psetname, empty_PSet);

    // check that this destination is not just a placeholder // change log 11
    bool is_placeholder 
	= dest_pset.getBool("placeholder", false);
    if (is_placeholder) continue;

    // Modify the file name if extension or name is explicitly specified
    // change log 14 

    // Although for an ordinary destination there is no output attribute
    // for the cfg (you can use filename instead) we provide output() for
    // uniformity with the statistics destinations.  The "right way" to
    // work this would have been to provide a filename() method, along with 
    // an extension() method.  We recognize the potential name confusion here
    // (filename(filename))!
    
    // grab the destination type
    String dest_type = dest_pset.getString("type", "file");

    // Determine the destination file name to use if no explicit filename is
    // supplied in the cfg.
    String filename_default = dest_pset.getString("output", empty_String);

    if ( filename_default == empty_String ) {
      filename_default = messageLoggerDefaults->output(psetname); 
                                   // change log 31
      if (filename_default  == empty_String) {
        filename_default  = filename;
      }        
    }

    String explicit_filename 
        = dest_pset.getString("filename", filename_default);
    String explicit_extension 
        = dest_pset.getString("extension", empty_String);

    filename = explicit_filename;

    if (explicit_extension != empty_String) 
    {
      if (explicit_extension[0] == '.')
	filename = filename + explicit_extension;             
      else
	filename = filename + "." + explicit_extension;   
    }

    // Attach a default extension of .log if there is no extension on a file
    if (filename.find('.') == std::string::npos)
        filename += ".log";

    // Make up a stream_id for duplication checking
    std::string stream_id;

    if(dest_type == "file")         stream_id = filename;
    else if(dest_type == "cout")    stream_id = std::string("cout");
    else if(dest_type == "cerr")    stream_id = std::string("cerr");
    else                            stream_id = dest_type + "_" + psetname;
    
    // Check that this is not a duplicate name			// change log 18
    if ( stream_ps.find(stream_id)!=stream_ps.end() ) {        
      if (clean_slate_configuration) {				// change log 22
//        throw mf::Exception ( mf::errors::Configuration )   
        LogError("duplicateDestination")			// change log 35
        <<"Duplicate name for a MessageLogger Destination: " 
        << stream_id
        << "\n" << "Only the first configuration instructions are used";
	continue;
      } else {
        LogWarning("duplicateDestination")
        <<"Duplicate name for a MessageLogger Destination: " 
        << stream_id
        << "\n" << "Only original configuration instructions are used";
        continue;
      }
    } 
    
    // push the stream_id to ordinary dest filename 
    if(dest_type=="file" || dest_type=="cout" || dest_type=="cerr")
      ordinary_destination_filenames.push_back(stream_id);

    // attach the current destination, keeping a control handle to it:
    ELdestControl dest_ctrl;
    std::ostream* os_p;
    if( dest_type == "cout" )  
    {
      os_p = &std::cout;
      dest_ctrl = admin_p->attach( ELoutput(std::cout) );
      stream_ps["cout"] = &std::cout;
    }
    else if( dest_type == "cerr" )  
    {
      os_p = &std::cerr;
      early_dest.setThreshold(ELzeroSeverity); 
      dest_ctrl = early_dest;
      stream_ps["cerr"] = &std::cerr;
    }
    else if( dest_type == "file" ) 
    {
      bool append = dest_pset.getBool("append", false);
      boost::shared_ptr<std::ofstream> 
          os_sp(new std::ofstream(
                  filename.c_str(), 
                  append ? std::ios_base::app : std::ios_base::trunc));
      file_ps.push_back(os_sp);
      os_p = os_sp.get();
      dest_ctrl = admin_p->attach( ELoutput(*os_sp) );
      stream_ps[filename] = os_sp.get();
    }
    else  
    {
      // destinations from Extension package
      boost::scoped_ptr<ELdestination> dest_sp(
          ELdestinationFactory::createInstance(
              dest_type, psetname, dest_pset) );

      if(dest_sp.get() == 0) 
      {
        LogError("ExtensionNotFound")
            << "The destination of type \"" 
            << dest_type
            << "\" does not exist!";
        continue;
      }

      dest_ctrl = admin_p->attach( *dest_sp );
    } 

    // now configure this destination:
    configure_dest(dest_ctrl, psetname, dest_pset);

    // check if this destination outputs the statistics
    bool output_stat = dest_pset.getBool("outputStatistics", false);

    // build and configure statistic destinations
    if( output_stat )
    {
      if( dest_type == "cout" || dest_type == "cerr" || dest_type == "file" )
      {
        ELdestControl stat_ctrl;
        stat_ctrl = admin_p->attach( ELstatistics(*os_p) );

        statisticsDestControls.push_back(stat_ctrl);
        statisticsResets.push_back( dest_pset.getBool("resetStatistics", false) );

        configure_dest(stat_ctrl, psetname, dest_pset);

        stat_ctrl.noTerminationSummary();
      }
      else
      {
        // statistic destination for extension types not supported yet
      }
    }

    //(*errorlog_p)( ELinfo, "added_dest") << filename << endmsg;

  }  // for [it = destinations.begin() to end()]

#if 0
  // grab list of destinations:
  vString  destinations
     = job_pset_p->getVString("destinations", empty_vString);

  // Use the default list of destinations if and only if the grabbed list is
  // empty						 	// change log 24
  if (destinations.empty()) {
    destinations = messageLoggerDefaults->destinations;
  }
  
  // dial down the early destination if other dest's are supplied:
  if( ! destinations.empty() )
    early_dest.setThreshold(ELhighestSeverity);

  // establish each destination:
  for( vString::const_iterator it = destinations.begin()
     ; it != destinations.end()
     ; ++it
     )
  {
    String filename = *it;
    String psetname = filename;
    
    // check that this destination is not just a placeholder // change log 11
    PSet  dest_pset = job_pset_p->getPSet(psetname, empty_PSet);
    bool is_placeholder 
	= dest_pset.getBool("placeholder", false);
    if (is_placeholder) continue;

    // Modify the file name if extension or name is explicitly specified
    // change log 14 

    // Although for an ordinary destination there is no output attribute
    // for the cfg (you can use filename instead) we provide output() for
    // uniformity with the statistics destinations.  The "right way" to
    // work this would have been to provide a filename() method, along with 
    // an extension() method.  We recognize the potential name confusion here
    // (filename(filename))!
    
    // Determine the destination file name to use if no explicit filename is
    // supplied in the cfg.
    String filename_default 
        = dest_pset.getString("output", empty_String);
    if ( filename_default == empty_String ) {
      filename_default = messageLoggerDefaults->output(psetname); // change log 31
      if (filename_default  == empty_String) {
        filename_default  = filename;
      }        
    }

    String explicit_filename 
        = dest_pset.getString("filename", filename_default);
    if (explicit_filename != empty_String) filename = explicit_filename;
    String explicit_extension 
        = dest_pset.getString("extension", empty_String);
    if (explicit_extension != empty_String) {
      if (explicit_extension[0] == '.') {
	filename += explicit_extension;             
      } else {
	filename = filename + "." + explicit_extension;   
      }
    }

    // Attach a default extension of .log if there is no extension on a file
    // change log 18 - this had been done in concert with attaching destination
    
    std::string actual_filename = filename;			// change log 4
    if ( (filename != "cout") 
        && (filename != "cerr") 
        && (filename.find('|',0) == std::string::npos) )  {
      const std::string::size_type npos = std::string::npos;
      if ( filename.find('.') == npos ) {
        actual_filename += ".log";
      }  
    }

     // Check that this is not a duplicate name			// change log 18
    if ( stream_ps.find(actual_filename)!=stream_ps.end() ) {        
      if (clean_slate_configuration) {				// change log 22
//        throw mf::Exception ( mf::errors::Configuration )   
        LogError("duplicateDestination")			// change log 35
        <<"Duplicate name for a MessageLogger Destination: " 
        << actual_filename
        << "\n" << "Only the first configuration instructions are used";
	continue;
      } else {
        LogWarning("duplicateDestination")
        <<"Duplicate name for a MessageLogger Destination: " 
        << actual_filename
        << "\n" << "Only original configuration instructions are used";
        continue;
      }
    } 
    
    ordinary_destination_filenames.push_back(actual_filename);

    // attach the current destination, keeping a control handle to it:
    ELdestControl dest_ctrl;
    if( actual_filename == "cout" )  {
      dest_ctrl = admin_p->attach( ELoutput(std::cout) );
      stream_ps["cout"] = &std::cout;
    }
    else if( actual_filename == "cerr" )  {
      early_dest.setThreshold(ELzeroSeverity); 
      dest_ctrl = early_dest;
      stream_ps["cerr"] = &std::cerr;
    }
    else  {
      // distinguish remote destinations from local file destinations
      std::string::size_type seprator = actual_filename.find('|', 0);

      if( seprator != std::string::npos) 
      {

        std::string extension_type = actual_filename.substr(0, seprator);
        std::string extension_name = actual_filename.substr(seprator+1);

        const std::string::size_type npos = std::string::npos;
        if ( extension_name.find('.') == npos ) {
          extension_name += ".log";
        }  

        // grab all of this destination's parameters:
        PSet dest_pset = job_pset_p->getPSet( 
            actual_filename, empty_PSet);

        boost::scoped_ptr<ELdestination> dest_sp(
            ELdestinationFactory::createInstance(
                extension_type, extension_name, dest_pset) );

        if(dest_sp.get() == 0) 
        {
          LogError("ExtensionNotFound")
              << "Extension specified in " 
              << actual_filename 
              << " does not exist!\n";
          continue;
        }

        dest_ctrl = admin_p->attach( *dest_sp );
      } 
      else 
      {
        boost::shared_ptr<std::ofstream> os_sp(new std::ofstream(actual_filename.c_str()));
        file_ps.push_back(os_sp);
        dest_ctrl = admin_p->attach( ELoutput(*os_sp) );
        stream_ps[actual_filename] = os_sp.get();
      }
    }
    //(*errorlog_p)( ELinfo, "added_dest") << filename << endmsg;

    // now configure this destination:
    configure_dest(dest_ctrl, psetname);

  }  // for [it = destinations.begin() to end()]
#endif

} // configure_ordinary_destinations


void
  MessageLoggerScribe::configure_statistics()
{
  vString  empty_vString;
  String   empty_String;
  PSet     empty_PSet;

  // grab list of statistics destinations:
  PSet     dests      = job_pset_p->getPSet("destinations");
  PSet     stats      = dests.getPSet("statistics");
  vString  statistics = stats.getPSetNameList();
  
  bool no_statistics_configured = statistics.empty();		// change log 24
  
  if ( no_statistics_configured ) {
    // Read the list of staistics destinations from hardwired defaults,
    // but only if there is also no list of ordinary destinations.
    // (If a cfg specifies destinations, and no statistics, assume that
    // is what the user wants.)
    vString  destinations = dests.getPSetNameList();
    if (destinations.empty()) { 
      statistics = messageLoggerDefaults->statistics;
      no_statistics_configured = statistics.empty();
    }
  }

   // establish each statistics destination:
  for( vString::const_iterator it = statistics.begin()
     ; it != statistics.end()
     ; ++it
     )
  {
    String psetname = *it;
    String filename = psetname;

    PSet stat_pset = stats.getPSet(psetname, empty_PSet);

    // check that this destination is not just a placeholder // change log 20
    bool is_placeholder 
	= stat_pset.getBool("placeholder", false);
    if (is_placeholder) continue;

    // grab the statistic destination type
    String dest_type = stat_pset.getString("type", "file");

    // Determine the destination file name to use if no explicit filename is
    // supplied in the cfg.
    String filename_default = stat_pset.getString("output", empty_String);

    if ( filename_default == empty_String ) {
      filename_default = messageLoggerDefaults->output(psetname); 
      if (filename_default  == empty_String) {
        filename_default  = filename;
      }        
    }

    String explicit_filename 
        = stat_pset.getString("filename", filename_default);
    String explicit_extension 
        = stat_pset.getString("extension", empty_String);

    filename = explicit_filename;

    if (explicit_extension != empty_String) 
    {
      if (explicit_extension[0] == '.')
	filename = filename + explicit_extension;             
      else
	filename = filename + "." + explicit_extension;   
    }

    // Attach a default extension of .log if there is no extension on a file
    if (filename.find('.') == std::string::npos)
        filename += ".log";

    // Make up a stream_id for duplication checking
    std::string stream_id;

    if(dest_type == "file")         stream_id = filename;
    else if(dest_type == "cout")    stream_id = std::string("cout");
    else if(dest_type == "cerr")    stream_id = std::string("cerr");
    else                            stream_id = dest_type + "_" + psetname;
 

    // Check that this is not a duplicate name - 
    // unless it is an ordinary destination (which stats can share)
    if ( !search_all(ordinary_destination_filenames, stream_id) )
    {
      if ( stream_ps.find(stream_id)!=stream_ps.end() ) 
      {        
        if (clean_slate_configuration) 
        {			// change log 22
          throw mf::Exception ( mf::errors::Configuration ) 
          <<"Duplicate name for a MessageLogger Statistics Destination: " 
          << stream_id
          << "\n";
        } 
        else 
        {
          LogWarning("duplicateDestination")
          <<"Duplicate name for a MessageLogger Statistics Destination: " 
          << stream_id
          << "\n" << "Only original configuration instructions are used";
          continue;
        } 
      }
    }
    
    // create (if statistics file does not match any destination file name)
    // or note (if statistics file matches a destination file name) the ostream.
    // But if no statistics destinations were provided in the config, do not
    // create a new destination for this hardwired statistics - only act if
    // it is matches a destination.  (shange log 24)
    bool statistics_destination_is_real = !no_statistics_configured;

    std::ostream* os_p;

    if ( stream_ps.find(stream_id) == stream_ps.end() ) 
    {
      if ( dest_type == "cout" ) 
      {
        os_p = &std::cout;
      } 
      else if ( dest_type == "cerr" ) 
      {
        os_p = &std::cerr;
      } 
      else if ( dest_type == "file" )
      {
        boost::shared_ptr<std::ofstream> 
            os_sp(new std::ofstream(stream_id.c_str()));
	file_ps.push_back(os_sp);
        os_p = os_sp.get();
      }
      else   // all other extension types
      {
        throw mf::Exception ( mf::errors::Configuration )
              << "Not supported statistic destination type \""
              << dest_type
              << "\"\n";
      }

      stream_ps[stream_id] = os_p;

    } 
    else 
    { 
      statistics_destination_is_real = true;			// change log 24
      os_p = stream_ps[stream_id];
    }
       
    if (statistics_destination_is_real)	{			// change log 24
      // attach the statistics destination, keeping a control handle to it:
      ELdestControl dest_ctrl;
      dest_ctrl = admin_p->attach( ELstatistics(*os_p) );
      statisticsDestControls.push_back(dest_ctrl);
      bool reset = stat_pset.getBool("reset", false);
      statisticsResets.push_back(reset);
    
      // now configure this destination:
      configure_dest(dest_ctrl, psetname, stat_pset);

      // and suppress the desire to do an extra termination summary just because
      // of end-of-job info messages
      dest_ctrl.noTerminationSummary();
    }
     
  }  // for [it = statistics.begin() to end()]


#if 0
  // grab list of statistics destinations:
  vString  statistics 
     = job_pset_p->getVString("statistics", empty_vString);
  
  bool no_statistics_configured = statistics.empty();		// change log 24
  
  if ( no_statistics_configured ) {
    // Read the list of staistics destinations from hardwired defaults,
    // but only if there is also no list of ordinary destinations.
    // (If a cfg specifies destinations, and no statistics, assume that
    // is what the user wants.)
    vString  destinations
     = job_pset_p->getVString("destinations", empty_vString);
    if (destinations.empty()) { 
      statistics = messageLoggerDefaults->statistics;
      no_statistics_configured = statistics.empty();
    }
  }

   // establish each statistics destination:
  for( vString::const_iterator it = statistics.begin()
     ; it != statistics.end()
     ; ++it
     )
  {
    String statname = *it;
    String psetname = statname;

    // check that this destination is not just a placeholder // change log 20
    PSet  stat_pset = job_pset_p->getPSet(psetname, empty_PSet);
    bool is_placeholder 
	= stat_pset.getBool("placeholder", false);
    if (is_placeholder) continue;

    // Determine the destination file name
    String filename 
        = stat_pset.getString("output", empty_String);
    if ( filename == empty_String ) {
      filename = messageLoggerDefaults->output(psetname);	// change log 31
      if (filename == empty_String) {
        filename = statname;
      }        
    }

    // Modify the file name if extension or name is explicitly specified
    // change log 14 -- probably suspenders and a belt, because ouput option
    // is present, but uniformity is nice.
					
    String explicit_filename 
        = stat_pset.getString("filename", filename);
    if (explicit_filename != empty_String) filename = explicit_filename;
    String explicit_extension 
        = stat_pset.getString("extension", empty_String);
    if (explicit_extension != empty_String) {
      if (explicit_extension[0] == '.') {
	filename += explicit_extension;             
      } else {
	filename = filename + "." + explicit_extension;   
      }
    }

    // Attach a default extension of .log if there is no extension on a file
    // change log 18 - this had been done in concert with attaching destination
    
    std::string actual_filename = filename;			// change log 4
    if ( (filename != "cout") && (filename != "cerr") )  {	// change log 23
      const std::string::size_type npos = std::string::npos;
      if ( filename.find('.') == npos ) {
        actual_filename += ".log";
      }  
    }

    // Check that this is not a duplicate name - 
    // unless it is an ordinary destination (which stats can share)
    if ( !search_all(ordinary_destination_filenames, actual_filename) ) {
      if ( stream_ps.find(actual_filename)!=stream_ps.end() ) {        
        if (clean_slate_configuration) {			// change log 22
          throw mf::Exception ( mf::errors::Configuration ) 
          <<"Duplicate name for a MessageLogger Statistics Destination: " 
          << actual_filename
          << "\n";
        } else {
          LogWarning("duplicateDestination")
          <<"Duplicate name for a MessageLogger Statistics Destination: " 
          << actual_filename
          << "\n" << "Only original configuration instructions are used";
          continue;
        } 
      }
    }
    
    // create (if statistics file does not match any destination file name)
    // or note (if statistics file matches a destination file name) the ostream.
    // But if no statistics destinations were provided in the config, do not
    // create a new destination for this hardwired statistics - only act if
    // it is matches a destination.  (shange log 24)
    bool statistics_destination_is_real = !no_statistics_configured;
    std::ostream* os_p;
    if ( stream_ps.find(actual_filename) == stream_ps.end() ) {
      if ( actual_filename == "cout" ) {
        os_p = &std::cout;
      } else if ( actual_filename == "cerr" ) {
        os_p = &std::cerr;
      } else {
        boost::shared_ptr<std::ofstream> os_sp(new std::ofstream(actual_filename.c_str()));
	file_ps.push_back(os_sp);
        os_p = os_sp.get();
      }
      stream_ps[actual_filename] = os_p;
    } else { 
      statistics_destination_is_real = true;			// change log 24
      os_p = stream_ps[actual_filename];
    }
       
    if (statistics_destination_is_real)	{			// change log 24
      // attach the statistics destination, keeping a control handle to it:
      ELdestControl dest_ctrl;
      dest_ctrl = admin_p->attach( ELstatistics(*os_p) );
      statisticsDestControls.push_back(dest_ctrl);
      bool reset = stat_pset.getBool("reset", false);
      statisticsResets.push_back(reset);
    
      // now configure this destination:
      configure_dest(dest_ctrl, psetname);

      // and suppress the desire to do an extra termination summary just because
      // of end-of-job info messages
      dest_ctrl.noTerminationSummary();
    }
     
  }  // for [it = statistics.begin() to end()]
#endif

} // configure_statistics

void
  MessageLoggerScribe::configure_external_dests()
{
  if( ! job_pset_p )  
  {
//  extern_dests.clear();				
//  change log 12, removed by change log 13
    return;
  }

  for( std::vector<NamedDestination*>::const_iterator it = extern_dests.begin()
     ; it != extern_dests.end()
     ;  ++it
     )
  {
    ELdestination *  dest_p = (*it)->dest_p().get();
    ELdestControl  dest_ctrl = admin_p->attach( *dest_p );

    // configure the newly-attached destination:
    //configure_dest( dest_ctrl, (*it)->name() );
    delete *it;  // dispose of our (copy of the) NamedDestination
  }
  extern_dests.clear();
 
}  // MessageLoggerScribe::configure_external_dests

std::string MessageLoggerScribe::trim_copy(std::string const src)
{
  std::string::size_type len = src.length();
  std::string::size_type i    = 0;
  std::string::size_type j    = len-1;

  while( (i < len) && (src[i] == ' ') ) ++i;
  while( (j > 0  ) && (src[j] == ' ') ) --j;

  return src.substr(i,j-i+1);
}

void
  MessageLoggerScribe::parseCategories (std::string const & s,
  				        std::vector<std::string> & cats)
{
  // Note:  This algorithm assigns, as desired, one null category if it
  //        encounters an empty categories string

  const std::string::size_type npos = s.length();
        std::string::size_type i    = 0;
  while ( i <= npos ) {    

    if(i==npos) {
      cats.push_back(std::string());
      return;
    }

    std::string::size_type j = s.find('|',i); 
    std::string cat = trim_copy(s.substr(i,j-i));  
    cats.push_back (cat);
    i = j;
    while ( (i < npos) && (s[i] == '|') ) ++i; 
    // the above handles cases of || and also | at end of string
  } 
}

void
  MessageLoggerScribe::triggerStatisticsSummaries() {
    assert (statisticsDestControls.size() == statisticsResets.size());
    for (unsigned int i = 0; i != statisticsDestControls.size(); ++i) {
      statisticsDestControls[i].summary( );
      if (statisticsResets[i]) statisticsDestControls[i].wipe( );
    }
}

void
  MessageLoggerScribe::
  triggerFJRmessageSummary(std::map<std::string, double> & sm)  // ChangeLog 29
{
  if (statisticsDestControls.empty()) {
    sm["NoStatisticsDestinationsConfigured"] = 0.0;
  } else {
    statisticsDestControls[0].summaryForJobReport(sm);
  }
}

//ErrorLog * MessageLoggerScribe::static_errorlog_p;


} // end of namespace service  
} // end of namespace mf  

