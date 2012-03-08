#ifndef MessageLogger_MessageLogger_h
#define MessageLogger_MessageLogger_h

// -*- C++ -*-
//
// Package:     MessageLogger
// Class  :     <none>
// Functions:   LogSystem,   LogError,   LogWarning, LogInfo,     LogDebug
//              LogAbsolute, LogProblem, LogPrint,   LogVerbatim, LogTrace
//			     LogImportant
//

//
// Original Author:  W. Brown and M. Fischler
//         Created:  Fri Nov 11 16:38:19 CST 2005
//     Major Split:  Tue Feb 14 11:00:00 CST 2006
//		     See MessageService/interface/MessageLogger.h
//
// =================================================
// Change log
//
// 1 mf 5/11/06	    Added a space before the file/line string in LogDebug_
//		    to avoid the run-together with the run and event number
//
// 2 mf 6/6/06	    Added LogVerbatim and LogTrace
//
// 3 mf 10/30/06    Added LogSystem and LogPrint
//
// 4 mf 6/1/07      Added LogAbsolute and LogProblem
//
// 5 mf 7/24/07     Added HaltMessageLogging
//
// 6 mf 8/7/07      Added FlushMessageLog
//
// 7 mf 8/7/07      Added GroupLogStatistics(category)
//
// 8 mf 12/12/07    Reworked LogDebug macro, LogDebug_class,, and similarly
//		    for LogTrace, to avoid the need for the static dummy
//		    objects.  This cures the use-of-thread-commands-after-
//		    exit problem in programs that link but do not use the
//		    MessageLogger.
// 
// 9  mf 12/12/07   Check for subtly terrible situation of copying and then
//		    writing to a LogDebug_ object.  Also forbid copying any
//		    of the ordinary LogXXX objects (since that implies either
//		    copying a MessageSender, or having a stale copy available
//		    to lose message contents).
//
// 10 mf 12/14/07   Moved the static free function onlyLowestDirectory
//		    to a class member function of LogDebug_, changing
//		    name to a more descriptive stripLeadingDirectoryTree.
//		    Cures the 2600-copies-of-this-function complaint.
//
// 11 mf  6/24/08   Added LogImportant which is LogProblem.  For output
//		    which "obviously" ought to emerge, but allowing specific
//		    suppression as if it were at the LogError level, rather
//		    than the non-suppressible LogAbsolute.
//
// 12 ge  9/12/08   MessageLogger now works even when compiled with -DNDEBUG.
//                  The problem was that Suppress_LogDebug_ was missing the operator<<
//                  needed for streaming `std::iomanip`s.
//
// 13 wmtan 11/18/08 Use explicit non-inlined destructors
//
// 14 mf  3/23/09   ap.get() used whenever possible suppression, to avoid
//		    null pointer usage
//
// 15 mf  8/11/09   provision for control of standalone threshold and ignores
//
// 16 mf  10/2/09  Correct mission in logVerbatim and others of check for
//		   whether this severity is enabled
//
// 17 wmtan 10/29/09 Out of line LogDebug_ and LogTrace_ constructors.
//
// 18 ql  02/16/10  Added StartMessageFacilityService(), SetModuleName(),
//                  SetContext() to simplify the start up commands of the 
//                  stand alone MessageFacility. A MessageFacilityService
//                  singleton class is established to hold the global handler.
//
// 19 ql  02/18/10  Modified LogError(), LogWarning(), LogInfo() to insert
//                  _FILE_ and _LINE_ in the message
//
// 20 ql  03/05/10  Added LogErrorObj(const ErrorObj & eo) to log a structured
//                  ErrorObj directly
//
// =================================================

// system include files

#include <memory>
#include <string>
#include <set>

#include "boost/shared_ptr.hpp"
#include "boost/scoped_ptr.hpp"

// user include files

// forward declarations

#include "messagefacility/MessageLogger/MessageSender.h"
#include "messagefacility/MessageLogger/MessageDrop.h"
#include "messagefacility/MessageLogger/MessageLoggerQ.h"	// Change log 5
#include "messagefacility/MessageLogger/ErrorObj.h"
#include "messagefacility/Utilities/exception.h"		// Change log 8

#include "messagefacility/MessageService/Presence.h"
#include "messagefacility/MessageService/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"

#include "boost/thread/mutex.hpp"


namespace mf  {

class LogWarning
{
public:
  explicit LogWarning( std::string const & id, std::string const & file="--", int line=0);
  ~LogWarning();						// Change log 13

  template< class T >
    LogWarning & 
    operator<< (T const & t)  { if(ap.get()) (*ap) << t; return *this; }
  LogWarning & 
  operator<< ( std::ostream&(*f)(std::ostream&))  
				      { if(ap.get()) (*ap) << f; return *this; }
  LogWarning & 
  operator<< ( std::ios_base&(*f)(std::ios_base&) )  
				      { if(ap.get()) (*ap) << f; return *this; }     
private:
  std::auto_ptr<MessageSender> ap; 
  LogWarning( LogWarning const& );				// Change log 9
   
};  // LogWarning_

//#define LogWarning(id)  ::mf::LogWarning_(id, __FILE__, __LINE__)
#define LOG_WARNING(id)  ::mf::LogWarning(id, __FILE__, __LINE__)


class LogError
{
public:
  explicit LogError( std::string const & id, std::string const & file="--", int line=0);
  ~LogError();							// Change log 13

  template< class T >
    LogError & 
    operator<< (T const & t)  { (*ap) << t; return *this; }
  LogError & 
  operator<< ( std::ostream&(*f)(std::ostream&))  
				      { (*ap) << f; return *this; }
  LogError & 
  operator<< ( std::ios_base&(*f)(std::ios_base&) )  
				      { (*ap) << f; return *this; }     

private:
  std::auto_ptr<MessageSender> ap; 
  LogError( LogError const& );				// Change log 9

};  // LogError

//#define LogError(id)  ::mf::LogError_(id, __FILE__, __LINE__)
#define LOG_ERROR(id)  ::mf::LogError(id, __FILE__, __LINE__)


class LogSystem
{
public:
  explicit LogSystem( std::string const & id, std::string const & file="--", int line=0 ) 
    : ap( new MessageSender(ELsevere,id) )
  { }
  ~LogSystem();							// Change log 13

  template< class T >
    LogSystem & 
    operator<< (T const & t)  { (*ap) << t; return *this; }
  LogSystem & 
  operator<< ( std::ostream&(*f)(std::ostream&))  
				      { (*ap) << f; return *this; }
  LogSystem & 
  operator<< ( std::ios_base&(*f)(std::ios_base&) )  
				      { (*ap) << f; return *this; }     

private:
  std::auto_ptr<MessageSender> ap; 
  LogSystem( LogSystem const& );				// Change log 9

};  // LogSystem

#define LOG_SYSTEM(id)  ::mf::LogSystem(id, __FILE__, __LINE__)

class LogInfo				
{
public:
  explicit LogInfo( std::string const & id, std::string const & file="--", int line=0);
  ~LogInfo();							// Change log 13

  template< class T >
    LogInfo & 
    operator<< (T const & t)  { if(ap.get()) (*ap) << t; return *this; }
  LogInfo & 
  operator<< ( std::ostream&(*f)(std::ostream&))  
				      { if(ap.get()) (*ap) << f; return *this; }
  LogInfo & 
  operator<< ( std::ios_base&(*f)(std::ios_base&) )  
				      { if(ap.get()) (*ap) << f; return *this; }     

private:
  std::auto_ptr<MessageSender> ap; 
  LogInfo( LogInfo const& );					// Change log 9
  
};  // LogInfo

//#define LogInfo(id)  ::mf::LogInfo_(id, __FILE__, __LINE__)
#define LOG_INFO(id)  ::mf::LogInfo(id, __FILE__, __LINE__)


// verbatim version of LogInfo
class LogVerbatim						// change log 2
{
public:
  explicit LogVerbatim( std::string const & id, std::string const & file="--", int line=0 ) 
    : ap ( mf::MessageDrop::instance()->infoEnabled ?		// change log 16
      new MessageSender(ELinfo,id,true) : 0 ) // the true is the verbatim arg 
  { }
  ~LogVerbatim();						// Change log 13

  template< class T >
    LogVerbatim & 
    operator<< (T const & t)  { if(ap.get()) (*ap) << t; return *this; } 
								// Change log 14
  LogVerbatim & 
  operator<< ( std::ostream&(*f)(std::ostream&))  
				      { if(ap.get()) (*ap) << f; return *this; }
  LogVerbatim & 
  operator<< ( std::ios_base&(*f)(std::ios_base&) )  
				      { if(ap.get()) (*ap) << f; return *this; }   

private:
  std::auto_ptr<MessageSender> ap; 
  LogVerbatim( LogVerbatim const& );				// Change log 9
  
};  // LogVerbatim

#define LOG_VERBATIM(id)  ::mf::LogVerbatim(id, __FILE__, __LINE__)

// verbatim version of LogWarning
class LogPrint							// change log 3
{
public:
  explicit LogPrint( std::string const & id, std::string const & file="--", int line=0 ) 
    : ap ( mf::MessageDrop::instance()->warningEnabled ?	// change log 16
      new MessageSender(ELwarning,id,true) : 0 ) // the true is the Print arg 
  { }
  ~LogPrint();							// Change log 13

  template< class T >
    LogPrint & 
    operator<< (T const & t)  { if(ap.get()) (*ap) << t; return *this; } 
								// Change log 14
  LogPrint & 
  operator<< ( std::ostream&(*f)(std::ostream&))  
				{ if(ap.get()) (*ap) << f; return *this; }
  LogPrint & 
  operator<< ( std::ios_base&(*f)(std::ios_base&) )  
				{ if(ap.get()) (*ap) << f; return *this; }      

private:
  std::auto_ptr<MessageSender> ap; 
  LogPrint( LogPrint const& );					// Change log 9
  
};  // LogPrint

#define LOG_PRINT(id)  ::mf::LogPrint(id, __FILE__, __LINE__)

// verbatim version of LogError
class LogProblem						// change log 4
{
public:
  explicit LogProblem( std::string const & id, std::string const & file="--", int line=0 ) 
    : ap( new MessageSender(ELerror,id,true) )
  { }
  ~LogProblem();						// Change log 13

  template< class T >
    LogProblem & 
    operator<< (T const & t)  { (*ap) << t; return *this; }
  LogProblem & 
  operator<< ( std::ostream&(*f)(std::ostream&))  
				      { (*ap) << f; return *this; }
  LogProblem & 
  operator<< ( std::ios_base&(*f)(std::ios_base&) )  
				      { (*ap) << f; return *this; }     

private:
  std::auto_ptr<MessageSender> ap; 
  LogProblem( LogProblem const& );				// Change log 9

};  // LogProblem

#define LOG_PROBLEM(id)  ::mf::LogProblem(id, __FILE__, __LINE__)

// less judgemental verbatim version of LogError
class LogImportant						// change log 11
{
public:
  explicit LogImportant( std::string const & id, std::string const & file="--", int line=0 ) 
    : ap( new MessageSender(ELerror,id,true) )
  { }
  ~LogImportant();						 // Change log 13

  template< class T >
    LogImportant & 
    operator<< (T const & t)  { (*ap) << t; return *this; }
  LogImportant & 
  operator<< ( std::ostream&(*f)(std::ostream&))  
				      { (*ap) << f; return *this; }
  LogImportant & 
  operator<< ( std::ios_base&(*f)(std::ios_base&) )  
				      { (*ap) << f; return *this; }     

private:
  std::auto_ptr<MessageSender> ap; 
  LogImportant( LogImportant const& );				// Change log 9

};  // LogImportant

#define LOG_IMPORTANT(id)  ::mf::LogImportant(id, __FILE__, __LINE__)

// verbatim version of LogSystem
class LogAbsolute						// change log 4
{
public:
  explicit LogAbsolute( std::string const & id, std::string const & file="--", int line=0 ) 
    : ap( new MessageSender(ELsevere,id,true) )
  { }
  ~LogAbsolute();						// Change log 13

  template< class T >
    LogAbsolute & 
    operator<< (T const & t)  { (*ap) << t; return *this; }
  LogAbsolute & 
  operator<< ( std::ostream&(*f)(std::ostream&))  
				      { (*ap) << f; return *this; }
  LogAbsolute & 
  operator<< ( std::ios_base&(*f)(std::ios_base&) )  
				      { (*ap) << f; return *this; }     

private:
  std::auto_ptr<MessageSender> ap; 
  LogAbsolute( LogAbsolute const& );				// Change log 9

};  // LogAbsolute

#define LOG_ABSOLUTE(id)  ::mf::LogAbsolute(id, __FILE__, __LINE__)

// change log 10:  removed onlyLowestDirectory()

void LogStatistics(); 
void LogErrorObj(ErrorObj * eo_p);

class LogDebug
{
public:
  explicit LogDebug( std::string const & id, std::string const & file="--", int line=0 ); // Change log 17
  explicit LogDebug()  : ap(), debugEnabled(false) {}		// Change log 8	
  ~LogDebug(); 

  template< class T >
    LogDebug & 
    operator<< (T const & t)  
    { if (!debugEnabled) return *this;				// Change log 8
      if (ap.get()) (*ap) << t; 
      else {
         Exception e(mf::errors::LogicError);
         e << "operator << to stale copied LogDebug_ object";
         throw e;
      }
      return *this; }
  LogDebug & 
  operator<< ( std::ostream&(*f)(std::ostream&))  
    { if (!debugEnabled) return *this;				// Change log 8
      if (ap.get()) (*ap) << f; 
      else {
         Exception e(mf::errors::LogicError);
         e << "operator << to stale copied LogDebug_ object";
         throw e;
      }
      return *this; }
  LogDebug & 
  operator<< ( std::ios_base&(*f)(std::ios_base&) )  
    { if (!debugEnabled) return *this;				// Change log 8
      if (ap.get()) (*ap) << f; 
      else {
         Exception e(mf::errors::LogicError);
         e << "operator << to stale copied LogDebug_ object";
         throw e;
      }
      return *this; }
			   // Change log 8:  The tests for ap.get() being null 

private:
  std::auto_ptr<MessageSender> ap; 
  bool debugEnabled;
  //std::string stripLeadingDirectoryTree (const std::string & file) const;
								// change log 10
};  // LogDebug

class LogTrace
{
public:
  explicit LogTrace( std::string const & id, std::string const & file="--", int line=0);			// Change log 13
  explicit LogTrace()  : ap(), debugEnabled(false) {}		// Change log 8	
  ~LogTrace(); 

  template< class T >
    LogTrace & 
    operator<< (T const & t)  
    { if (!debugEnabled) return *this;				// Change log 8
      if (ap.get()) (*ap) << t; 
      else {
         Exception e(mf::errors::LogicError);
         e << "operator << to stale copied LogTrace_ object";
         throw e;
      }
      return *this; }
  LogTrace & 
  operator<< ( std::ostream&(*f)(std::ostream&))  
    { if (!debugEnabled) return *this;				// Change log 8
      if (ap.get()) (*ap) << f; 
      else {
         Exception e(mf::errors::LogicError);
         e << "operator << to stale copied LogTrace_ object";
         throw e;
      }
      return *this; }
  LogTrace & 
  operator<< ( std::ios_base&(*f)(std::ios_base&) )  
    { if (!debugEnabled) return *this;				// Change log 8
      if (ap.get()) (*ap) << f; 
      else {
         Exception e(mf::errors::LogicError);
         e << "operator << to stale copied LogTrace_ object";
         throw e;
      }
      return *this; }
			   // Change log 8:  The tests for ap.get() being null 
 
private:
  std::auto_ptr<MessageSender> ap; 
  bool debugEnabled;
  
};  // LogTrace

extern LogDebug dummyLogDebugObject_;
extern LogTrace dummyLogTraceObject_;

class Suppress_LogDebug_ 
{ 
  // With any decent optimization, use of Suppress_LogDebug_ (...)
  // including streaming of items to it via operator<<
  // will produce absolutely no executable code.
public:
  template< class T >
    Suppress_LogDebug_ &operator<< (T const & t) { return *this; }	// Change log 12
    Suppress_LogDebug_ &operator<< (std::ostream&(*)(std::ostream&)) { return *this; }	// Change log 12
    Suppress_LogDebug_ &operator<< (std::ios_base&(*)(std::ios_base&)) { return *this; } // Change log 12
};  // Suppress_LogDebug_

  bool isDebugEnabled();
  bool isInfoEnabled();
  bool isWarningEnabled();
  void HaltMessageLogging();
  void FlushMessageLog();
  void GroupLogStatistics(std::string const & category);
  bool isMessageProcessingSetUp();

  // Change Log 15
  // The following two methods have no effect except in stand-alone apps
  // that do not create a MessageServicePresence:
  void setStandAloneMessageThreshold    (std::string const & severity);
  void squelchStandAloneMessageCategory (std::string const & category);


class MessageFacilityService
{
private:
  MessageFacilityService();

public:
  static MessageFacilityService & instance();

  static fhicl::ParameterSet logConsole( );
  static fhicl::ParameterSet logServer( );
  static fhicl::ParameterSet logFile(
                          std::string const & filename = "logfile"
                        , bool append = false
                        );
  static fhicl::ParameterSet logCS( );
  static fhicl::ParameterSet logCF(
                          std::string const & filename = "logfile"
                        , bool append = false
                        );
  static fhicl::ParameterSet logFS(
                          std::string const & filename = "logfile"
                        , bool append = false
                        );
  static fhicl::ParameterSet logCFS(
                          std::string const & filename = "logfile"
                        , bool append = false
                        );
  static fhicl::ParameterSet logArchive(
                          std::string const & filename = "msgarchive"
                        , bool append = false
                        );

  static fhicl::ParameterSet ConfigurationFile(
               std::string const & filename = "MessageFacility.cfg",
               fhicl::ParameterSet const & def = logCF());

  static std::string SingleThread;
  static std::string MultiThread;

  bool   MFServiceEnabled;

  boost::shared_ptr<Presence> MFPresence;
  boost::shared_ptr<service::MessageLogger> theML;

  boost::mutex m;

private:
  static std::string commonPSet();
};

class MFSdestroyer
{
public:
  ~MFSdestroyer();
};

  // Change log 18
  //

  void StartMessageFacility(
      std::string const & mode,
      fhicl::ParameterSet const & pset = MessageFacilityService::ConfigurationFile());

  void SetApplicationName(std::string const & application);
  void SetModuleName(std::string const & modulename);
  void SetContext(std::string const & context);

  void SwitchChannel(int c);

}  // namespace mf


// If ML_DEBUG is defined, LogDebug is active.  
// Otherwise, LogDebug is supressed if either ML_NDEBUG or NDEBUG is defined.
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#ifdef NDEBUG
#define EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#endif
#ifdef ML_NDEBUG
#define EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#endif
#ifdef ML_DEBUG
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#endif

#ifdef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG 
#define LOG_DEBUG(id) ::mf::Suppress_LogDebug_()
#define LOG_TRACE(id) ::mf::Suppress_LogDebug_()
#else
#define LOG_DEBUG(id)                                 \
  ( !::mf::MessageDrop::instance()->debugEnabled )    \
    ?  ::mf::LogDebug()                              \
    :  ::mf::LogDebug(id, __FILE__, __LINE__)
#define LOG_TRACE(id)                                 \
  ( !::mf::MessageDrop::instance()->debugEnabled )    \
    ?  ::mf::LogTrace()                              \
    :  ::mf::LogTrace(id)
#endif
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
							// change log 1, 2
							// change log 8 using
							// default ctors
#endif  // MessageLogger_MessageLogger_h

