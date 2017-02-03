#ifndef messagefacility_Auxiliaries_ELseverityLevel_h
#define messagefacility_Auxiliaries_ELseverityLevel_h


// ----------------------------------------------------------------------
//
// ELseverityLevel.h - declare objects that encode a message's urgency
//
//      Both frameworker and user will often pass one of the
//      instantiated severity levels to logger methods.
//
//      The only other methods of ELseverityLevel a frameworker
//      might use is to check the relative level of two severities
//      using operator< or the like.
//
// ----------------------------------------------------------------------

#include <string>

namespace mf {

  class ELseverityLevel;

  using ELslGen = ELseverityLevel();

  // ----------------------------------------------------------------------
  // ELslProxy class template:
  // ----------------------------------------------------------------------

  template<ELslGen ELgen>
  struct ELslProxy {

    operator ELseverityLevel() const;

    int         getLevel()    const;
    std::string getSymbol()   const;
    std::string getName()     const;
    std::string getInputStr() const;
    std::string getVarName()  const;

  };  // ELslProxy<ELslGen>

  // ----------------------------------------------------------------------
  // ELseverityLevel:
  // ----------------------------------------------------------------------

  class ELseverityLevel  {
  public:

    // ---  One ELseverityLevel is globally instantiated (see below)
    // ---  for each of the following levels:

    enum ELsev_ {
      ELsev_noValueAssigned = 0  // default returned by map when not found
        , ELsev_zeroSeverity         // threshold use only
        , ELsev_incidental           // flash this on a screen
        , ELsev_success              // report reaching a milestone
        , ELsev_info                 // information
        , ELsev_warning              // warning
        , ELsev_warning2             // more serious warning
        , ELsev_error                // error detected
        , ELsev_error2               // more serious error
        , ELsev_next                 // advise to skip to next event
        , ELsev_unspecified          // severity was not specified
        , ELsev_severe               // future results are suspect
        , ELsev_severe2              // more severe
        , ELsev_abort                // suggest aborting
        , ELsev_fatal                // strongly suggest aborting!
        , ELsev_highestSeverity      // threshold use only
        // -----
        , nLevels                    // how many levels?
        };  // ELsev_

    constexpr ELseverityLevel(ELsev_ lev = ELsev_unspecified);
    ELseverityLevel (std::string const& str);
    // str may match getSymbol, getName, getInputStr, or getVarName --
    // see accessors

    int cmp(ELseverityLevel e) const;

    int          getLevel()    const;
    std::string  getSymbol()   const;  // example: "-e"
    std::string  getName()     const;  // example: "Error"
    std::string  getInputStr() const;  // example: "ERROR"
    std::string  getVarName()  const;  // example: "ELerror"

    friend std::ostream& operator<< (std::ostream& os,
                                     ELseverityLevel const sev);

  private:
    int myLevel;
  };  // ELseverityLevel

  inline
  constexpr
  ELseverityLevel::ELseverityLevel(ELsev_ const level)
  : myLevel{level}
  {}

  // ----------------------------------------------------------------------
  // Declare the globally available severity objects,
  // one generator function and one proxy per non-default ELsev_:
  // ----------------------------------------------------------------------

  inline ELseverityLevel constexpr ELzeroSeverityGen() {return ELseverityLevel::ELsev_zeroSeverity;}
  inline ELseverityLevel constexpr ELincidentalGen() {return ELseverityLevel::ELsev_incidental;}
  inline ELseverityLevel constexpr ELsuccessGen() {return ELseverityLevel::ELsev_success;}
  inline ELseverityLevel constexpr ELinfoGen() {return ELseverityLevel::ELsev_info;}
  inline ELseverityLevel constexpr ELwarningGen() {return ELseverityLevel::ELsev_warning;}
  inline ELseverityLevel constexpr ELwarning2Gen() {return ELseverityLevel::ELsev_warning2;}
  inline ELseverityLevel constexpr ELerrorGen() {return ELseverityLevel::ELsev_error;}
  inline ELseverityLevel constexpr ELerror2Gen() {return ELseverityLevel::ELsev_error2;}
  inline ELseverityLevel constexpr ELnextEventGen() {return ELseverityLevel::ELsev_next;}
  inline ELseverityLevel constexpr ELunspecifiedGen() {return ELseverityLevel::ELsev_unspecified;}
  inline ELseverityLevel constexpr ELsevereGen() {return ELseverityLevel::ELsev_severe;}
  inline ELseverityLevel constexpr ELsevere2Gen() {return ELseverityLevel::ELsev_severe2;}
  inline ELseverityLevel constexpr ELabortGen() {return ELseverityLevel::ELsev_abort;}
  inline ELseverityLevel constexpr ELfatalGen() {return ELseverityLevel::ELsev_fatal;}
  inline ELseverityLevel constexpr ELhighestSeverityGen() {return ELseverityLevel::ELsev_highestSeverity;}

  ELslProxy< ELzeroSeverityGen    > constexpr ELzeroSeverity {};
  ELslProxy< ELincidentalGen      > constexpr ELincidental {};
  ELslProxy< ELsuccessGen         > constexpr ELsuccess {};
  ELslProxy< ELinfoGen            > constexpr ELinfo {};
  ELslProxy< ELwarningGen         > constexpr ELwarning {};
  ELslProxy< ELwarning2Gen        > constexpr ELwarning2 {};
  ELslProxy< ELerrorGen           > constexpr ELerror {};
  ELslProxy< ELerror2Gen          > constexpr ELerror2 {};
  ELslProxy< ELnextEventGen       > constexpr ELnextEvent {};
  ELslProxy< ELunspecifiedGen     > constexpr ELunspecified {};
  ELslProxy< ELsevereGen          > constexpr ELsevere {};
  ELslProxy< ELsevere2Gen         > constexpr ELsevere2 {};
  ELslProxy< ELabortGen           > constexpr ELabort {};
  ELslProxy< ELfatalGen           > constexpr ELfatal {};
  ELslProxy< ELhighestSeverityGen > constexpr ELhighestSeverity {};


  // ----------------------------------------------------------------------
  // Comparators:
  // ----------------------------------------------------------------------

  extern bool operator==(ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator!=(ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator< (ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator<=(ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator> (ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator>=(ELseverityLevel e1, ELseverityLevel e2);

} // end of namespace mf


// ----------------------------------------------------------------------

#define ELSEVERITYLEVEL_ICC
#include "messagefacility/Utilities/ELseverityLevel.icc"
#undef  ELSEVERITYLEVEL_ICC

// ----------------------------------------------------------------------

#endif /* messagefacility_Auxiliaries_ELseverityLevel_h */

// Local variables:
// mode: c++
// End: