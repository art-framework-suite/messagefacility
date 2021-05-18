#include "cetlib_except/exception.h"

#include <iostream>
#include <string>

struct Thing : public cet::exception {
  Thing(const std::string& msg) : exception("Thing", msg) {}
};

std::ostream&
operator<<(std::ostream& os, const Thing& t)
{
  os << "Thing(" << t.explain_self() << ")";
  return os;
}

[[noreturn]] void
func3()
{
  throw Thing("Data Corrupt") << " Low level error" << std::endl;
}

[[noreturn]] void
func2()
{
  func3();
}

[[noreturn]] void
func1() try {
  func2();
}
catch (cet::exception& e) {
  throw cet::exception("InfiniteLoop", "In func2", e) << "Gave up";
}

int
main() try {
  func1();
}
catch (cet::exception& e) {
  std::cerr << "*** main caught cet::exception, output is ***\n"
            << "(" << e.explain_self() << ")"
            << "*** After exception output ***" << std::endl;

  std::cerr << "\nCategory name list:\n";
}
