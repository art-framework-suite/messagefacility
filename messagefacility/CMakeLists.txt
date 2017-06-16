include(CetMake)

cet_make_library(
  LIBRARY_NAME
  MF_MessageLogger
  SOURCE
  MessageLogger/MessageLogger.cc
  MessageLogger/MessageLoggerImpl.cc
  MessageLogger/MessageSender.cc
  MessageLogger/Presence.cc
  MessageLogger/MessageLoggerScribe.cc
  MessageService/ELadministrator.cc
  MessageService/ELdestination.cc
  MessageService/ELostreamOutput.cc
  MessageService/ELstatistics.cc
  MessageService/MessageDrop.cc
  MessageService/MessageLoggerQ.cc
  MessageService/MsgFormatSettings.cc
  MessageService/MsgStatistics.cc
  MessageService/ThreadQueue.cc
  LIBRARIES
  MF_Utilities
  cetlib_except
  fhiclcpp
  ${Boost_THREAD_LIBRARY}
  ${TBB}
  )

add_subdirectory(MessageLogger)
add_subdirectory(MessageService)
add_subdirectory(Utilities)
add_subdirectory(plugins)
# Testing
#add_subdirectory(test)
