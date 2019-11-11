#ifndef CAVCOM_UTILITY_LIBLOG_DATUM_H_
#define CAVCOM_UTILITY_LIBLOG_DATUM_H_

#include <string>

namespace cavcom {
  namespace utility {

    // A simple piece of data.
    template <typename T> class Datum {
     public:
      using value_type = T;

      // Creates a new piece of named data.
      explicit Datum(const std::string &name, const value_type &value = value_type())
        : name_(name), value_(value) {}

      // Returns the name associated with the datum.
      const std::string &name(void) { return name_; }

      // Gets/sets the data value.
      const value_type &value() const { return value_; }
      void value(const value_type &value) { value_ = value; }

      // Resets the value back to an initialized state.
      void reset(void) { value_ = value_type(); }

     private:
      const std::string name_;
      value_type value_;
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_DATUM_H_
