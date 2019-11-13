#ifndef CAVCOM_UTILITY_LIBLOG_SAMPLE_H_
#define CAVCOM_UTILITY_LIBLOG_SAMPLE_H_

#include "datum.h"
#include "sum.h"

namespace cavcom {
  namespace utility {
    // Forward reference.
    template <typename D> class CSVSampleFields;

    // Monitors the total, count, mean, min, and max of a sampled value.
    template <typename T> class Sample {
     public:
      using value_type = T;

      // Creates a new sampled value.  The various datum names are created by appending the above tags to the
      // specified name.
      explicit Sample(const std::string &name)
        : total_(name + "_total"),
          count_(name + "_count"),
          mean_(name + "_mean"),
          min_(name + "_min"),
          max_(name + "_max") {}

      // Adds a new sample and updates the values accordingly.
      void add(const value_type &sample) {
        total_.add(sample);
        count_.add(1);
        mean_.value(static_cast<double>(total_.value())/count_.value());
        if (count_.value() == 1) {
          min_.value(sample);
          max_.value(sample);
        } else {
          if (sample < min_.value()) min_.value(sample);
          if (sample > max_.value()) max_.value(sample);
        }
      }

      // Returns the various values.
      const value_type &total(void) const { return total_.value(); }
      const Counter::value_type &count(void) const { return count_.value(); }
      const double &mean(void) const { return mean_.value(); }
      const value_type &min(void) const { return min_.value(); }
      const value_type &max(void) const { return max_.value(); }

      // Resets all values.
      void reset(void) {
        total_.reset();
        count_.reset();
        mean_.reset();
        min_.reset();
        max_.reset();
      }

     private:
      Sum<value_type> total_;
      Counter count_;
      Datum<double> mean_;
      Datum<value_type> min_;
      Datum<value_type> max_;

      friend class CSVSampleFields<T>;
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_SAMPLE_H_
