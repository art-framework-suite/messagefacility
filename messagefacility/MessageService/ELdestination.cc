//======================================================================
//
// ELdestination
//
//======================================================================

#include "fhiclcpp/types/Table.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageService/ELdestConfigCheck.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/Utilities/bold_fontify.h"

#include <fstream>
#include <iostream>
#include <iomanip>

std::string const
cet::PluginTypeDeducer<mf::service::ELdestination>::
value = "ELdestination";

using namespace std::string_literals;

namespace mf {
  namespace service {

    namespace {
      std::string const noSummarizationMsg {"No summarization()"};
      std::string const noSummaryMsg {"No summary()"};
      std::string const hereMsg {"available via this destination"};
      std::string const noosMsg {"No ostream"};
      std::string const preamble {"%MSG"};
    }

    //=============================================================================

    ELdestination::ELdestination(Config const& pset)
      : stats{pset.msgStatistics()}
      , format{pset.format()}
      , threshold{pset.threshold()}
      , lineLength_{pset.noLineBreaks() ? 32000ull : pset.lineLength()}
      , enableStats{pset.outputStatistics()}
    {
      // Modify automatic suppression if necessary.
      if (threshold <= ELseverityLevel::ELsev_success)
      { MessageDrop::debugAlwaysSuppressed = false; }
      if (threshold <= ELseverityLevel::ELsev_info)
      { MessageDrop::infoAlwaysSuppressed = false; }
      if (threshold <= ELseverityLevel::ELsev_warning)
      { MessageDrop::warningAlwaysSuppressed = false; }

      if (pset.noTimeStamps()) {
        format.suppress(TIMESTAMP);
      }

      if (pset.useMilliseconds()) {
        format.include(MILLISECOND);
      }

      configure(pset.categories);
    }

    //=============================================================================
    void ELdestination::emitToken(std::ostream& os, std::string const& s, bool const nl)
    {
      if (s.empty()) {
        if (nl)  {
          os << '\n';
          charsOnLine = 0;
        }
        return;
      }

      char const first = s[0];
      char const second = (s.length() < 2) ? '\0' : s[1];
      char const last = (s.length() < 2) ? '\0' : s[s.length()-1];
      char const last2 = (s.length() < 3) ? '\0' : s[s.length()-2];
      // checking -2 because the very last char is sometimes a ' '
      // inserted by ErrorLog::operator<<

      if (format.preambleMode) {

        //Accounts for newline @ the beginning of the std::string
        if (first == '\n' || (charsOnLine + static_cast<int>(s.length())) > lineLength_) {
          charsOnLine = 0;
          if (second != ' ') {
            os << ' ';
            charsOnLine++;
          }
          if (first == '\n') {
            os << s.substr(1);
          }
          else {
            os << s;
          }
        }
        else {
          os << s;
        }

        if (last == '\n' || last2 == '\n') {  // accounts for newline @ end
          os << indent;                       // of the std::string
          if (last != ' ')
            os << ' ';
          charsOnLine = indent.length() + 1;
        }

        if (nl) { os << '\n'; charsOnLine = 0;           }
        else    {             charsOnLine += s.length(); }
      }

      if (!format.preambleMode) {
        os << s;
      }

    }  // emitToken()

    //=============================================================================
    bool ELdestination::passLogStatsThreshold(mf::ErrorObj const& msg) const
    {
      // See if this message is to be counted.
      if (msg.xid().severity() < threshold) return false;
      if (thisShouldBeIgnored(msg.xid().module())) return false;

      return true;
    }

    //=============================================================================
    bool ELdestination::passLogMsgThreshold(mf::ErrorObj const& msg)
    {
      auto const& xid = msg.xid();

      // See if this message is to be acted upon, and add it to limits table
      // if it was not already present:
      if (xid.severity() < threshold)  return false;
      if (xid.severity() < ELsevere && thisShouldBeIgnored(xid.module())) return false;
      if (xid.severity() < ELsevere && !stats.limits.add(xid)) return false;

      return true;
    }

    void ELdestination::fillPrefix(std::ostringstream& oss,
                                   mf::ErrorObj const& msg)
    {
      if (msg.is_verbatim()) return;

      // Output the prologue:
      //
      format.preambleMode = true;

      auto const& xid = msg.xid();

      charsOnLine = 0;
      emitToken(oss, preamble);
      emitToken(oss, xid.severity().getSymbol());
      emitToken(oss, " ");
      emitToken(oss, xid.id());
      emitToken(oss, msg.idOverflow());
      emitToken(oss, ": ");

      // Output serial number of message:
      //
      if (format.want(SERIAL)) {
        std::ostringstream s;
        s << msg.serial();
        emitToken(oss, "[serial #" + s.str() + "] ");
      }

      // Provide further identification:
      //
      bool needAspace = true;
      if (format.want(EPILOGUE_SEPARATE)) {
        if (xid.module().length()+xid.subroutine().length() > 0) {
          emitToken(oss,"\n");
          needAspace = false;
        }
        else if (format.want(TIMESTAMP) && !format.want(TIME_SEPARATE)) {
          emitToken(oss,"\n");
          needAspace = false;
        }
      }
      if (format.want(MODULE) && (xid.module().length() > 0)) {
        if (needAspace) {
          emitToken(oss," ");
          needAspace = false;
        }
        emitToken(oss, xid.module() + " ");
      }
      if (format.want(SUBROUTINE) && (xid.subroutine().length() > 0)) {
        if (needAspace) {
          emitToken(oss," ");
          needAspace = false;
        }
        emitToken(oss, xid.subroutine() + "() ");
      }

      // Provide time stamp:
      //
      if (format.want(TIMESTAMP))  {
        if (format.want(TIME_SEPARATE))  {
          emitToken(oss, "\n");
          needAspace = false;
        }
        if (needAspace) {
          emitToken(oss," ");
          needAspace = false;
        }
        emitToken(oss, format.timestamp(msg.timestamp()) + " ");
      }

      // Provide the context information:
      //
      if (format.want(SOME_CONTEXT)) {
        if (needAspace) {
          emitToken(oss," ");
          needAspace = false;
        }
        emitToken(oss, msg.context());
      }
    }

    //=============================================================================
    void ELdestination::fillUsrMsg(std::ostringstream& oss, mf::ErrorObj const& msg)
    {
      if (!format.want(TEXT)) return;

      format.preambleMode = false;
      auto const usrMsgStart = std::next(msg.items().cbegin(), 4);
      auto it = msg.items().cbegin();

      // Determine if file and line should be included
      if (!msg.is_verbatim()) {

        // The first four items are { " ", "<FILENAME>", ":", "<LINE>" }
        while (it != usrMsgStart) {
          if (!it->compare(" ") && !std::next(it)->compare("--")) {
            // Do not emitToken if " --:0" is the match
            std::advance(it,4);
          }
          else {
            // Emit if <FILENAME> and <LINE> are meaningful
            emitToken(oss, *it++);
          }
        }

        // Check for user-requested line breaks
        if (format.want(NO_LINE_BREAKS)) emitToken(oss, " ==> ");
        else emitToken(oss, "", true);
      }

      // For verbatim (and user-supplied) messages, just print the contents
      auto const end = msg.items().cend();
      for (; it != end; ++it) {
        emitToken(oss, *it);
      }

    }

    //=============================================================================
    void ELdestination::fillSuffix(std::ostringstream& oss,
                                   mf::ErrorObj const& msg)
    {
      if (!msg.is_verbatim() && !format.want(NO_LINE_BREAKS)) {
        emitToken(oss,"\n%MSG");
      }
      oss << '\n';
    }

    //=============================================================================
    void ELdestination::routePayload(std::ostringstream const&,
                                     mf::ErrorObj const&)
    {}

    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator:
    // ----------------------------------------------------------------------

    //=============================================================================
    void ELdestination::log(mf::ErrorObj& msgObj)
    {
      if (!passLogMsgThreshold(msgObj)) return;

      std::ostringstream payload;
      fillPrefix(payload, msgObj);
      fillUsrMsg(payload, msgObj);
      fillSuffix(payload, msgObj);

      routePayload(payload, msgObj);

      msgObj.setReactedTo(true);

      if (enableStats && passLogStatsThreshold(msgObj))
        stats.log(msgObj);
    }

    // Each of the functions below must be overridden by any
    // destination for which they make sense.  In this base class,
    // where they are all no-ops, the methods which generate data to a
    // destination, stream or stream will warn at that place, and all
    // the no-op methods will issue an ELwarning2 at their own
    // destination.

    void ELdestination::wipe()
    {
      stats.limits.wipe();
    }

    void ELdestination::respondToModule(std::string const& moduleName)
    {
      if (moduleName=="*") {
        ignoreMostModules = false;
        respondToMostModules = true;
        ignoreThese.clear();
        respondToThese.clear();
      } else {
        respondToThese.insert(moduleName);
        ignoreThese.erase(moduleName);
      }
    }

    void ELdestination::ignoreModule(std::string const& moduleName)
    {
      if (moduleName=="*") {
        respondToMostModules = false;
        ignoreMostModules = true;
        respondToThese.clear();
        ignoreThese.clear();
      } else {
        ignoreThese.insert(moduleName);
        respondToThese.erase(moduleName);
      }
    }

    void ELdestination::filterModule(std::string const& moduleName)
    {
      ignoreModule("*");
      respondToModule(moduleName);
    }

    void ELdestination::excludeModule(std::string const& moduleName)
    {
      respondToModule("*");
      ignoreModule(moduleName);
    }

    void ELdestination::summary()
    {
      if (enableStats && stats.updatedStats && stats.printAtTermination)
        {
          std::ostringstream payload;
          payload << "\n=============================================\n\n"
                  << "MessageLogger Summary\n"
                  << stats.formSummary();
          routePayload(payload, mf::ErrorObj{ELzeroSeverity, noosMsg});
        }
    }

    void ELdestination::summary(std::ostream& os, std::string const& title)
    {
      os << preamble
         << ELwarning.getSymbol() << " "
         << noSummaryMsg << " "
         << hereMsg << '\n'
         << title << '\n';
    }

    void ELdestination::summary(std::string& s, std::string const& title)
    {
      std::ostringstream ss;
      summary(ss, title);
      s = ss.str();
    }

    void ELdestination::finish()
    {}

    void ELdestination::setThreshold(ELseverityLevel const sv)
    {
      threshold = sv;
    }

    void ELdestination::summarization(std::string const& title,
                                      std::string const& /*sumfines*/)
    {
      mf::ErrorObj msg {ELwarning, noSummarizationMsg};
      msg << hereMsg << '\n' << title;
      log(msg);
    }

    void ELdestination::flush()
    {
      mf::ErrorObj msg {ELwarning, noosMsg};
      msg << "cannot flush()";
      log(msg);
    }

    // ----------------------------------------------------------------------
    // Protected helper methods:
    // ----------------------------------------------------------------------

    bool ELdestination::thisShouldBeIgnored(std::string const& s) const
    {
      if (respondToMostModules) {
        return ignoreThese.find(s) != ignoreThese.end();
      }
      else if (ignoreMostModules) {
        return respondToThese.find(s) == cend(respondToThese);
      }
      else {
        return false;
      }
    }

    void
    ELdestination::configure(fhicl::OptionalDelegatedParameter const& cat_config)
    {
      std::vector<std::string> configuration_errors;

      // Grab this destination's category configurations.
      fhicl::ParameterSet cats_pset{};
      cat_config.get_if_present<fhicl::ParameterSet>(cats_pset);

      // Grab the list of categories, removing the default category
      // since it is handled specially.
      auto const default_category_name = "default"s;
      auto categories = cats_pset.get_pset_names();
      auto erase_from = std::remove_if(begin(categories), end(categories),
                                       [&default_category_name](auto const& category) {
                                         return category == default_category_name;
                                       });
      categories.erase(erase_from, categories.cend());

      // Setup the default configuration for categories--this involves
      // resetting the limits table according to the user-specified
      // default configuration.
      auto const& default_pset = cats_pset.get<fhicl::ParameterSet>(default_category_name, {});
      try {
        fhicl::WrappedTable<Category::Config> default_params{default_pset};
        stats.limits = ELlimitsTable{default_params().limit(), default_params().reportEvery(), default_params().timespan()};
      }
      catch (fhicl::detail::validationException const& e) {
        std::string msg {"Category: " + detail::bold_fontify(default_category_name) + "\n\n"};
        msg += e.what();
        configuration_errors.push_back(std::move(msg));
      }

      // Now establish this destination's limit/interval/timespan for
      // each category, using the values of the possibly-specified
      // default configuration when a given category is missing the
      // fields.
      for (auto const& category : categories) {
        fhicl::Table<Category::Config> category_params{fhicl::Name{category}, default_pset};
        try {
          category_params.validate_ParameterSet(cats_pset.get<fhicl::ParameterSet>(category));
        }
        catch (fhicl::detail::validationException const& e) {
          std::string msg {"Category: " + detail::bold_fontify(category) + "\n\n"};
          msg += e.what();
          configuration_errors.push_back(std::move(msg));
        }

        stats.limits.setCategory(category,
                                 category_params().limit(),
                                 category_params().reportEvery(),
                                 category_params().timespan());
      }

      if (!configuration_errors.empty()) {
        std::string msg{"The following categories were misconfigured:\n\n"};
        for (auto const& error : configuration_errors) {
          msg += error;
        }
        throw fhicl::detail::validationException{msg};
      }

    }

  } // end of namespace service
} // end of namespace mf
