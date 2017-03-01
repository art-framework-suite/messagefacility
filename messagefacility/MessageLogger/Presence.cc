#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageLogger/ThreadSafeLogMessageLoggerScribe.h"
#include "messagefacility/MessageLogger/Presence.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"

#include <memory>

namespace mf {
  Presence::Presence()
  {
    MessageLoggerQ::setMLscribe_ptr(std::make_unique<service::ThreadSafeLogMessageLoggerScribe>());
    MessageDrop::instance()->messageLoggerScribeIsRunning = MLSCRIBE_RUNNING_INDICATOR;
  }

  Presence::~Presence()
  {
    MessageLoggerQ::MLqEND();
    MessageLoggerQ::setMLscribe_ptr(nullptr);
  }
} // end of namespace mf
