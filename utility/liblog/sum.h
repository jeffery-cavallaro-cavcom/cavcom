#ifndef CAVCOM_UTILITY_LIBLOG_SUM_H_
#define CAVCOM_UTILITY_LIBLOG_SUM_H_

#include "types.h"
#include "datum.h"

namespace cavcom {
  namespace utility {

    // A datum for a summed item.
    template <typename T> class Sum : public Datum<T> {
     public:
      using value_type = T;

      // Creates a new, initialized sum.
      explicit Sum(const std::string &name) : Datum<T>(name) {}

      // Gets/sets the data value.
      virtual const value_type &value(void) const { return Datum<T>::value(); }
      virtual void value(const value_type &value) { Datum<T>::value(value); }

      // Adds the specified amount to the sum.
      void add(const value_type &amount) { value(value() + amount); }
    };

    // A specialization for a peg counter.
    using Counter = Sum<ullong>;

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_SUM_H_
