Log messages
============


Messaging logging interfaces
----------------------------

There are four message logging methods corresponding to four message severities: :red:`ERROR`, :yellow:`WARNING`, :green:`INFO`, and :grey:`DEBUG`. 
Each method has two different versions: one embedded with contains debug info ("__file__", and "__line__"), and the other not. 
So there are total 8 methods that one may choose from to report status,::

    // Log messages without __file__ and __line__ info:
    mf::LogError   ( std::string const & category ) << a << b << ... << z ;
    mf::LogWarning ( std::string const & category ) << a << b << ... << z ;
    mf::LogInfo    ( std::string const & category ) << a << b << ... << z ;
    mf::LogDebug   ( std::string const & category ) << a << b << ... << z ;
    
    // Log messages with embedded debug info:
    LOG_ERROR   ( std::string const & category ) << a << b << ... << z ;
    LOG_WARNING ( std::string const & category ) << a << b << ... << z ;
    LOG_INFO    ( std::string const & category ) << a << b << ... << z ;
    LOG_DEBUG   ( std::string const & category ) << a << b << ... << z ;


Note that the debug version of function calls (:blue:`LOG_ERROR`, etc) are actually macros, therefore do not have the namespace in front of them.

:blue:`category` is a string representing the category (e.g., "MemoryIssue" or "NetworkConnection") of the issuing message. 
It is allowed to use combined categories connected with a vertical bar "|" in one string ("MemoryIssue | NetworkConnection").

Each function call is followed by one or more streaming operators to form the message body.

Here is a complete list of the macro versions for logging messages.::

    #define LOG_ABSOLUTE(id)  ::mf::LogAbsolute(id, __FILE__, __LINE__)
    #define LOG_ERROR(id)     ::mf::LogError(id, __FILE__, __LINE__)
    #define LOG_IMPORTANT(id) ::mf::LogImportant(id, __FILE__, __LINE__)
    #define LOG_INFO(id)      ::mf::LogInfo(id, __FILE__, __LINE__)
    #define LOG_PROBLEM(id)   ::mf::LogProblem(id, __FILE__, __LINE__)
    #define LOG_PRINT(id)     ::mf::LogPrint(id, __FILE__, __LINE__)
    #define LOG_SYSTEM(id)    ::mf::LogSystem(id, __FILE__, __LINE__)
    #define LOG_VERBATIM(id)  ::mf::LogVerbatim(id, __FILE__, __LINE__)
    #define LOG_WARNING(id)   ::mf::LogWarning(id, __FILE__, __LINE__)
    
    #ifdef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
      #define LOG_DEBUG(id) true ? ::mf::NeverLogger_() : ::mf::NeverLogger_()
      #define LOG_TRACE(id) true ? ::mf::NeverLogger_() : ::mf::NeverLogger_()
    #else
      #define LOG_DEBUG(id)   ! ::mf::MessageDrop::instance()->debugEnabled \
                            ? ::mf::LogDebug(id, __FILE__, __LINE__) \
                            : ::mf::LogDebug(id, __FILE__, __LINE__)
      #define LOG_TRACE(id)   ! ::mf::MessageDrop::instance()->debugEnabled \
                            ? ::mf::LogTrace(id, __FILE__, __LINE__) \
                            : ::mf::LogTrace(id, __FILE__, __LINE__)


Examples
--------

::

    mf::LogWarning( "DataIntegrity" ) << "dcm-05 has detected total " << nEmptySlices << " of empty microslices";


Message logging over containers
-------------------------------

It is possible to use logging with containers (i.e. vector-like objects) without having to create a new message for each container entry. The following can be done:::

    // Example with vector of ints
    std::vector<int> vint{ { 1, 2, 5, 89, 3 } };
    
    // Create non-temporary mf::LogInfo instance
    mf::LogInfo linfo("info");
    linfo << " vint contains: ";
    
    auto i = std::begin(vint);
    auto const e = std::end(vint);
    while (i != e) {
     linfo << *i;
     if (++i != e) {
       linfo << ", ";
     }
    }

which will give an output of (e.g.):::

    %MSG-i info:  function 26-Sep-2014 15:25:49 CDT context
      vint contains: 1, 2, 5, 89, 3
    %MSG


Ordering of messages
--------------------

Consider the following example:::

    {
       mf::LogDebug("info") << "Debug 1";
    
       mf::LogDebug debug2("info");
       debug2 << "Debug 2";
    
       mf::LogDebug debug3("info"); 
       debug3 << "Debug 3";
    
       mf::LogDebug("info") << "Debug 4";
       mf::LogDebug("info") << "Debug 5";
    
       mf::LogStatistics();
    
    }

The printout of this code is (omitting lines beginning with %MSG):::

    Debug 1
    Debug 4
    Debug 5
    <Statistics print out EXCLUDING debug2 and debug3>
    Debug 3
    Debug 2

The reason for this behavior has to do with whether the mf::LogDebug object created is a temporary one or non-temporary one. 
For an mf::Log object, the message is logged once the object's destructor is called. That means for cases like:::

       mf::LogDebug("info") << "Debug statement"; // This is a temporary object

the destructor is called (i.e. the message is logged) immediately once the above expression is evaluated. 
However, for non-temporary objects, the objects persist until the end of the scope in which they are defined, at which point the destructors of the persistent objects are called in reverse order. 
There are two ways the above behavior can present a problem:

- if the ordering of messages is necessary (note that each message has its own timestamp, so message ordering may not be important)
- if statistics output is desired and mf::LogStatistics() is called in the same scope as a non-temporary mf::Log object.

The correct behavior can be ensured by placing the non-temporary objects in their own scopes:::

    {
       mf::LogDebug("info") << "Debug 1";
    
       {
          mf::LogDebug debug2("info");
          debug2 << "Debug 2";
       } // debug2 destructor called
    
       {
          mf::LogDebug debug3("info"); 
          debug3 << "Debug 3";
       } // debug3 destructor called
    
       mf::LogDebug("info") << "Debug 4";
       mf::LogDebug("info") << "Debug 5";
    
       mf::LogStatistics();
    
    }


yielding the desired result:::

    Debug 1
    Debug 2
    Debug 3
    Debug 4
    Debug 5
    <Statistics print out INCLUDING debug2 and debug3>


Interpreting the codes within log messages
==========================================

Here is the table of symbols associated with the messages.  So -s means severe severity, -i means info severity.::

    case ELsev_zeroSeverity   :  result =  "--"; break;
    case ELsev_incidental     :  result =  ".."; break;
    case ELsev_success        :  result =  "-d"; break;         // 4/21/09 mf
    case ELsev_info           :  result =  "-i"; break;
    case ELsev_warning        :  result =  "-w"; break;
    case ELsev_warning2       :  result =  "-W"; break;
    case ELsev_error          :  result =  "-e"; break;
    case ELsev_error2         :  result =  "-E"; break;
    case ELsev_next           :  result =  "-n"; break;
    case ELsev_unspecified    :  result =  "??"; break;
    case ELsev_severe         :  result =  "-s"; break;
    case ELsev_severe2        :  result =  "-S"; break;
    case ELsev_abort          :  result =  "-A"; break;
    case ELsev_fatal          :  result =  "-F"; break;
    case ELsev_highestSeverity:  result =  "!!"; break;



