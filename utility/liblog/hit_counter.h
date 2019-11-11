#ifndef CAVCOM_UTILITY_LIBLOG_HIT_COUNTER_H_
#define CAVCOM_UTILITY_LIBLOG_HIT_COUNTER_H_

#include "sum.h"
#include "percent.h"

namespace cavcom {
  namespace utility {

    // Records the number of tries and the number of hits/successes for a particular counted event.
    class HitCounter {
     public:
      // Creates a new, initialized hit counter.  The specified name will be appended with "_tries" and "_hits"
      // for the two counter value names, and "_pct" for the hit percentage value.
      explicit HitCounter(const std::string &name);

      // Adds the specified amount of tries and hits and recalculates the percentage.
      void add(const Counter::value_type &tries, const Counter::value_type &hits);

      // Returns the current number of tries and hits, and a percentage of successful hits.
      const Counter::value_type &tries(void) const { return tries_.value(); }
      const Counter::value_type &hits(void) const { return hits_.value(); }
      const Percent::value_type &percent(void) const { return percent_.value(); }

      // Resets all values.
      void reset(void);

     private:
      Counter tries_;
      Counter hits_;
      Percent percent_;
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_HIT_COUNTER_H_
