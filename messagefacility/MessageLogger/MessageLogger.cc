#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageLogger/MessageDrop.h"

#include "messagefacility/MessageService/MessageServicePresence.h"
#include "messagefacility/MessageService/ELadministrator.h"

#include "cetlib/filepath_maker.h"
#include "fhiclcpp/make_ParameterSet.h"

#include <string>
#include <fstream>

// Change Log
//
// 12/12/07  mf   elimination of dummyLogDebugObject_, dummyLogTraceObject_
//               (see change log 8 in MessageLogger.h)
//
// 12/14/07  mf  Moved the static free function onlyLowestDirectory
//               to a class member function of LogDebug_, changing
//               name to a more descriptive stripLeadingDirectoryTree.
//               Cures the 2600-copies-of-this-function complaint.
//               Implementation of this is moved into this .cc file.
//
//  6/20/08  mf  Have flushMessageLog() check messageLoggerScribeIsRunning
//               (in the message drop) to avoid hangs if that thread is not
//               around.
//
//  11/18/08 wmtan  Use explicit non-inlined destructors
//
//  8/11/09  mf setStandAloneMessageThreshold() and
//              squelchStandAloneMessageCategory()
//
//  10/29/09 wmtan  Use explicit non-inlined constructors for LogDebug_ and LogTrace_
//
// ------------------------------------------------------------------------

namespace {
  std::string stripLeadingDirectoryTree(const std::string & file);
}  

namespace mf {

LogInfo::~LogInfo() {}
LogWarning::~LogWarning() {}
LogError::~LogError() {}
LogAbsolute::~LogAbsolute() {}
LogSystem::~LogSystem() {}
LogVerbatim::~LogVerbatim() {}
LogDebug::~LogDebug() {}
LogTrace::~LogTrace() {}
LogPrint::~LogPrint() {}
LogProblem::~LogProblem() {}
LogImportant::~LogImportant() {}

void LogStatistics() {
  mf::MessageLoggerQ::MLqSUM ( ); // trigger summary info
}

void LogErrorObj(ErrorObj * eo_p) {
  mf::MessageLoggerQ::MLqLOG(eo_p);
}

bool isDebugEnabled() {
  return ( mf::MessageDrop::instance()->debugEnabled );
}

bool isInfoEnabled() {
  return( mf::MessageDrop::instance()->infoEnabled );
}

bool isWarningEnabled() {
  return( mf::MessageDrop::instance()->warningEnabled );
}

void HaltMessageLogging() {
  mf::MessageLoggerQ::MLqSHT ( ); // Shut the logger up
}

void FlushMessageLog() {
  if (MessageDrop::instance()->messageLoggerScribeIsRunning !=
                        MLSCRIBE_RUNNING_INDICATOR) return;     // 6/20/08 mf
  mf::MessageLoggerQ::MLqFLS ( ); // Flush the message log queue
}

bool isMessageProcessingSetUp() {                               // 6/20/08 mf
//  std::cerr << "isMessageProcessingSetUp: \n";
//  std::cerr << "messageLoggerScribeIsRunning = "
//          << (int)MessageDrop::instance()->messageLoggerScribeIsRunning << "\n";
  return (MessageDrop::instance()->messageLoggerScribeIsRunning ==
                        MLSCRIBE_RUNNING_INDICATOR);
}

void GroupLogStatistics(std::string const & category) {
  std::string * cat_p = new std::string(category);
  mf::MessageLoggerQ::MLqGRP (cat_p); // Indicate a group summary category
  // Note that the scribe will be responsible for deleting cat_p
}

std::string stripLeadingDirectoryTree(const std::string & file)
{
  std::string::size_type lastSlash = file.find_last_of('/');
  if (lastSlash == std::string::npos) return file;
  if (lastSlash == file.size()-1)     return file;
  return file.substr(lastSlash+1, file.size()-lastSlash-1);
}

// LogWarning
mf::LogWarning::LogWarning( std::string const & id, std::string const & file, int line )
    : ap ( mf::MessageDrop::instance()->warningEnabled ?
           new MessageSender(ELwarning,id) : 0 )
{
  *this << " "
        << stripLeadingDirectoryTree(file)
        << ":" << line << "\n";
}

// LogError
mf::LogError::LogError( std::string const & id, std::string const & file, int line )
    : ap( new MessageSender(ELerror,id) )
{
  *this << " "
        << stripLeadingDirectoryTree(file)
        << ":" << line << "\n";
}

// LogInfo
mf::LogInfo::LogInfo( std::string const & id, std::string const & file, int line )
    : ap ( mf::MessageDrop::instance()->infoEnabled ?
           new MessageSender(ELinfo,id) : 0 )
{
  *this << " "
        << stripLeadingDirectoryTree(file)
        << ":" << line << "\n";
}

// LogDebug
mf::LogDebug::LogDebug( std::string const & id, std::string const & file, int line )
  : ap( new MessageSender(ELsuccess,id) ), debugEnabled(true)
{ *this
        << " "
        << stripLeadingDirectoryTree(file)
        << ":" << line << "\n"; }

// LogTrace
mf::LogTrace::LogTrace( std::string const & id, std::string const & file, int line )
  : ap( new MessageSender(ELsuccess,id,true) )
  , debugEnabled(true)
  {  }

void setStandAloneMessageThreshold(std::string const & severity) {
  mf::MessageLoggerQ::standAloneThreshold(severity);
}
void squelchStandAloneMessageCategory(std::string const & category){
  mf::MessageLoggerQ::squelch(category);
}


// MessageFacilityService
std::string MessageFacilityService::SingleThread = "SingleThreadMSPresence";
std::string MessageFacilityService::MultiThread = "MessageServicePresence";

MessageFacilityService & MessageFacilityService::instance()
{
  static MessageFacilityService mfs;
  return mfs;
}

// Read configurations from MessageFacility.cfg file
fhicl::ParameterSet MessageFacilityService::ConfigurationFile(
    std::string const & filename,
    fhicl::ParameterSet const & def)
{
  size_t sub_start = filename.find("${");
  size_t sub_end   = filename.find("}");

  const size_t npos = std::string::npos;

  if(    (sub_start==npos && sub_end!=npos)
    || (sub_start!=npos && sub_end==npos)
    || (sub_start > sub_end)
    || (sub_start!=0 && sub_start!=npos) )
  {
    std::cout << "Unrecognized configuration file. "
              << "Use default configuration instead.\n";
    return def;
  }

  std::string fname;
  std::auto_ptr<cet::filepath_maker> policy_ptr(new cet::filepath_maker());

  if (sub_start==0)  // env embedded in the filename
  {
    std::string env = filename.substr(2, sub_end-2);
    fname = filename.substr(sub_end+1);
    policy_ptr.reset(new cet::filepath_lookup(env));
  }
  else if (filename.find('/')==0)  // absolute path
  {
    fname = filename;
    policy_ptr.reset(new cet::filepath_maker());
  }
  else                             // non-absolute path
  {
    std::string env = std::string("FHICL_FILE_PATH");
    fname = filename;
    policy_ptr.reset(new cet::filepath_lookup(env));
  }

  fhicl::ParameterSet pset;
  try {
    fhicl::make_ParameterSet(fname, *policy_ptr, pset);
    return pset;
  }
  catch (cet::exception &e) {
    std::cerr << "Configuration file \"" << fname << "\" "
              << "parsing failed with exception " << e.what()
              << ".\n"
              << "Default configuration will be used instead.\n";
  }

  return def;
}


std::string MessageFacilityService::commonPSet()
{
  std::string pset = "debugModules:[\"*\"]  statistics:[\"stats\"] ";
  return pset;
}

fhicl::ParameterSet MessageFacilityService::logConsole()
{
  std::ostringstream ss;
  ss << commonPSet()
     << "  destinations : { "
     << "    console : { type : \"cout\" threshold : \"DEBUG\" } "
     << "  } " ;

  fhicl::ParameterSet pset;
  std::string pstr(ss.str());
  fhicl::make_ParameterSet(pstr, pset);
  return pset;
}

fhicl::ParameterSet MessageFacilityService::logServer()
{
  std::ostringstream ss;
  ss << commonPSet()
     << "  destinations : { "
     << "    server : { "
     << "      type : \"dds\" threshold : \"DEBUG\" "
     << "    } "
     << "  } " ;

  fhicl::ParameterSet pset;
  std::string pstr(ss.str());
  fhicl::make_ParameterSet(pstr, pset);
  return pset;
}

fhicl::ParameterSet MessageFacilityService::logFile(std::string const & filename, bool append)
{
  std::ostringstream ss;
  ss << commonPSet()
     << "  destinations : { "
     << "    file : { "
     << "      type : \"file\" threshold : \"DEBUG\" "
     << "      filename : \"" << filename << "\" "
     << "      append : " << (append ? "true" : "false")
     << "    } "
     << "  } " ;

  fhicl::ParameterSet pset;
  std::string pstr(ss.str());
  fhicl::make_ParameterSet(pstr, pset);
  return pset;
}

fhicl::ParameterSet MessageFacilityService::logCS()
{
  std::ostringstream ss;
  ss << commonPSet()
     << "  destinations : { "
     << "    console : { type : \"cout\" threshold : \"DEBUG\" } "
     << "    server : { "
     << "      type : \"dds\" threshold : \"DEBUG\" "
     << "    } "
     << "  } " ;

  fhicl::ParameterSet pset;
  std::string pstr(ss.str());
  fhicl::make_ParameterSet(pstr, pset);
  return pset;
}

fhicl::ParameterSet MessageFacilityService::logCF(std::string const & filename, bool append)
{
  std::ostringstream ss;
  ss << commonPSet()
     << "  destinations : { "
     << "    console : { type : \"cout\" threshold : \"DEBUG\" } "
     << "    file : { "
     << "      type : \"file\" threshold : \"DEBUG\" "
     << "      filename : \"" << filename << "\" "
     << "      append : " << (append ? "true" : "false")
     << "    } "
     << "  } ";

  fhicl::ParameterSet pset;
  std::string pstr(ss.str());
  fhicl::make_ParameterSet(pstr, pset);
  return pset;
}

fhicl::ParameterSet MessageFacilityService::logFS(std::string const & filename, bool append)
{
  std::ostringstream ss;
  ss << commonPSet()
     << "  destinations : { "
     << "    file : { "
     << "      type : \"file\" threshold : \"DEBUG\" "
     << "      filename : \"" << filename << "\" "
     << "      append : " << (append ? "true" : "false")
     << "    } "
     << "    server : { "
     << "      type : \"dds\" threshold : \"DEBUG\" "
     << "    } "
     << "  } " ;

  fhicl::ParameterSet pset;
  std::string pstr(ss.str());
  fhicl::make_ParameterSet(pstr, pset);
  return pset;
}

fhicl::ParameterSet MessageFacilityService::logCFS(std::string const & filename, bool append)
{
  std::ostringstream ss;
  ss << commonPSet()
     << "  destinations : { "
     << "    console : { type : \"cout\" threshold : \"DEBUG\" } "
     << "    file : { "
     << "      type : \"file\" threshold : \"DEBUG\" "
     << "      filename : \"" << filename << "\" "
     << "      append : " << (append ? "true" : "false")
     << "    } "
     << "    server : { "
     << "      type : \"dds\" threshold : \"DEBUG\" "
     << "    } "
     << "  } " ;

  fhicl::ParameterSet pset;
  std::string pstr(ss.str());
  fhicl::make_ParameterSet(pstr, pset);
  return pset;
}

fhicl::ParameterSet MessageFacilityService::logArchive(std::string const & filename, bool append)
{
  std::ostringstream ss;
  ss << commonPSet()
     << "  destinations : { "
     << "    archive : { "
     << "      type : \"archive\" threshold : \"DEBUG\" "
     << "      filename : \"" << filename << "\" "
     << "      append : " << (append ? "true" : "false")
     << "    } "
     << "  } " ;

  fhicl::ParameterSet pset;
  std::string pstr(ss.str());
  fhicl::make_ParameterSet(pstr, pset);
  return pset;
}

MessageFacilityService::MessageFacilityService()
  : MFServiceEnabled  (false)
  , theML             (     )
{
}

MFSdestroyer::~MFSdestroyer()
{
  MessageFacilityService::instance().MFPresence.reset();
}

// Start MessageFacility service
void StartMessageFacility(
      std::string const & mode,
      fhicl::ParameterSet const & pset)
{
  MessageFacilityService & mfs = MessageFacilityService::instance();

  boost::mutex::scoped_lock lock(mfs.m);

  if( !mfs.MFServiceEnabled )
  {

    /*
     * qlu 03/10/10 The order of object initialization and destruction is
     *              curcial in starting up and shutting down the Message
     *              Facility service. In the d'tor of MessageServicePresence
     *              it sends out a END message to the queue and waits for the
     *              MLscribe thread to finish logging all remaining messages
     *              in the queue. Therefore the ELadministrator singleton
     *              (whose instance is handled by a local static variable)
     *              and all attached destinations must be present during the
     *              process. We must provide the secured method to gurantee
     *              that the MessageServicePresence will be destroyed first,
     *              and particularly *BEFORE* the destruction of ELadmin.
     *              This is achieved by instantiating a static object, who
     *              is responsible for killing the Presence at the *END* of
     *              the start sequence. So this destroyer object will be killed
     *              before everyone else.
     */

    // MessageServicePresence
    mfs.MFPresence.reset( PresenceFactory::createInstance(mode) );

    // The MessageLogger
    mfs.theML.reset( new service::MessageLogger(pset) );

    mfs.MFServiceEnabled = true;

    static MFSdestroyer destroyer;
  }
}

// Set application name
void SetApplicationName(std::string const & application)
{
  MessageFacilityService & mfs = MessageFacilityService::instance();

  if( ! mfs.MFServiceEnabled )    return;

  boost::mutex::scoped_lock lock(mfs.m);

  mf::service::ELadministrator::instance()->setApplication(application);
  SetModuleName(application);
}

// Set module name and debug settings
void SetModuleName(std::string const & modulename)
{
  if( !MessageFacilityService::instance().MFServiceEnabled )
    return;

  MessageDrop * drop = MessageDrop::instance();
  drop -> moduleName = modulename;

  MessageFacilityService & mfs = MessageFacilityService::instance();

  if( mfs.theML->everyDebugEnabled_ )
    drop -> debugEnabled = true;
  else if( mfs.theML->debugEnabledModules_.count(modulename))
    drop -> debugEnabled = true;
  else
    drop -> debugEnabled = false;
}

// Set the run/event context
void SetContext(std::string const & context)
{
  if( !MessageFacilityService::instance().MFServiceEnabled )
    return;

  MessageDrop::instance() -> runEvent = context;
}

// Switch dds partition / channel
void SwitchChannel(int c)
{
  if( c<0 || c>4 )
  {
    LogWarning("RemoteMsg") << "Specified channel dose not exist";
    return;
  }

  std::stringstream ss;
  ss << "Partition" << c;

  std::string * chanl = new std::string(ss.str());
  mf::MessageLoggerQ::MLqSWC(chanl);
}

}  // namespace mf
