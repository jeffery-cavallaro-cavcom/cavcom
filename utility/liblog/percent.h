#ifndef CAVCOM_UTILITY_LIBLOG_PERCENT_H_
#define CAVCOM_UTILITY_LIBLOG_PERCENT_H_

#include <cmath>

#include "datum.h"

namespace cavcom {
  namespace utility {

    // A percent value that is entered as a fractional value but is stored and reported as a number from 0 to 100
    // with 3 significant figures (e.g. 98.6).
    class Percent : public Datum<double> {
     public:
      // Creates a new percent datum.
      explicit Percent(const std::string &name) : Datum(name) {}

      // Adjusts and sets the data value.
      virtual const value_type &value(void) const { return Datum::value(); }

      virtual void value(const value_type &value) {
        Datum::value(std::round(value*1000.0)/10.0);
      }
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_PERCENT_H_
