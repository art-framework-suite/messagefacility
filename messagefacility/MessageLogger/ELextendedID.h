#ifndef MessageLogger_ELextendedID_h
#define MessageLogger_ELextendedID_h


// ----------------------------------------------------------------------
//
// ELextendedID.h       is used as a key in maps for both counting toward
//                      limits on how many times a destination will react
//                      to a type of message, and for statistics.
//
// 07-Jul-1998 mf       Created file.
// 26-Aug-1998 WEB      Updated with ELseverityLevel in place of int.
//
// ----------------------------------------------------------------------


#include "messagefacility/MessageLogger/ELseverityLevel.h"


namespace mf {


// ----------------------------------------------------------------------
// ELextendedID:
// ----------------------------------------------------------------------

class ELextendedID  {

public:

  // -----  Publicly accessible data members:
  //
  std::string        process;
  std::string        id;
  ELseverityLevel severity;
  std::string        module;
  std::string        subroutine;

  std::string        hostname;
  std::string        hostaddr;
  std::string        application;
  long            pid;

  // -----  Comparator:
  //
  bool operator<( const ELextendedID & xid ) const;

  // -----  (Re)initializer:
  //
  void clear();

};  // ELextendedID


// ----------------------------------------------------------------------


}        // end of namespace mf


#endif  // MessageLogger_ELextendedID_h
