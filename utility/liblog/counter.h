#ifndef CAVCOM_UTILITY_LIBLOG_COUNTER_H_
#define CAVCOM_UTILITY_LIBLOG_COUNTER_H_

#include <string>

#include "types.h"
#include "datum.h"

namespace cavcom {
  namespace utility {

    // A datum for a counted item (like an event peg counter).
    class Counter : public Datum<ullong> {
     public:
      // Creates a new, initialized counter.
      explicit Counter(const std::string &name, const value_type &value = 0) : Datum(name, value) {}

      // Adds the specified amount to the counter (default = 1).
      void add(const value_type &amount = 1) { value(value() + amount); }
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_COUNTER_H_
