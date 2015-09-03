// ----------------------------------------------------------------------
//
// ELmap.cc
//
// Change History:
//   99-06-10   mf      correction in sense of comparison between timespan
//                      and diff (now, lastTime)
//              mf      ELcountTRACE made available
//   99-06-11   mf      Corrected logic for suppressing output when n > limit
//                      but not but a factor of 2**K
//   06-05-16   mf      Added code to establish interval and to use skipped
//                      and interval when determinine in add() whehter to react
//   06-05-19   wmtan   Bug fix.  skipped = 0, not skipped == 0.
//                      and interval when determinine in add() whehter to react
//   09-04-15   wmtan   Use smart pointers with new, not bare pointers
//
// ----------------------------------------------------------------------


#include "messagefacility/MessageLogger/ELmap.h"

namespace mf
{


// ----------------------------------------------------------------------
// LimitAndTimespan:
// ----------------------------------------------------------------------

LimitAndTimespan::LimitAndTimespan( int lim, int ts, int ivl )
: limit   ( lim )
, timespan( ts )
, interval( ivl )
{ }


// ----------------------------------------------------------------------
// CountAndLimit:
// ----------------------------------------------------------------------

CountAndLimit::CountAndLimit( int lim, int ts, int ivl )
: n         ( 0 )
, aggregateN( 0 )
, lastTime  ( time(0) )
, limit     ( lim )
, timespan  ( ts  )
, interval  ( ivl )
, skipped   ( ivl-1 )  // So that the FIRST of the prescaled messages emerges
{ }


bool  CountAndLimit::add()  {

  time_t  now = time(0);

  // Has it been so long that we should restart counting toward the limit?
  if ( (timespan >= 0)
            &&
        (difftime(now, lastTime) >= timespan) )  {
     n = 0;
     if ( interval > 0 ) {
       skipped = interval - 1; // So this message will be reacted to
     } else {
       skipped = 0;
     }
  }

  lastTime = now;

  ++n;
  ++aggregateN;
  ++skipped;

  if (skipped < interval) return false;

  if ( limit == 0 ) return false;        // Zero limit - never react to this
  if ( (limit < 0)  || ( n <= limit )) {
    skipped = 0;
    return true;
  }

  // Now we are over the limit - have we exceeded limit by 2^N * limit?
  long  diff = n - limit;
  long  r = diff/limit;
  if ( r*limit != diff ) { // Not a multiple of limit - don't react
    return false;
  }
  if ( r == 1 )   {     // Exactly twice limit - react
    skipped = 0;
    return true;
  }

  while ( r > 1 )  {
    if ( (r & 1) != 0 )  return false;  // Not 2**n times limit - don't react
    r >>= 1;
  }
  // If you never get an odd number till one, r is 2**n so react

  skipped = 0;
  return true;

}  // add()


// ----------------------------------------------------------------------
// StatsCount:
// ----------------------------------------------------------------------

StatsCount::StatsCount()
: n          ( 0 )
, aggregateN ( 0 )
, ignoredFlag( false )
, context1   ( "" )
, context2   ( "" )
, contextLast( "" )
{ }


void  StatsCount::add( const ELstring & context, bool reactedTo )  {

  ++n;  ++aggregateN;

  ( (1 == n) ? context1
  : (2 == n) ? context2
  :            contextLast
  )                        = ELstring( context, 0, 16 );

  if ( ! reactedTo )
    ignoredFlag = true;

}  // add()


// ----------------------------------------------------------------------

} // end of namespace mf  */
