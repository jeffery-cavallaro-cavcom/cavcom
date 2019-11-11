#ifndef CAVCOM_UTILITY_LIBLOG_DATUM_H_
#define CAVCOM_UTILITY_LIBLOG_DATUM_H_

#include <string>

namespace cavcom {
  namespace utility {

    // A simple piece of data.
    template <typename T> class Datum {
     public:
      // Creates a new piece of named data.
      explicit Datum(const std::string &name, const T &value = T()) : name_(name), value_(value) {}

      // Returns the name associated with the datum.
      const std::string &name(void) { return name_; }

      // Gets/sets the data value.
      const T &value() const { return value_; }
      void value(const T &value) { value_ = value; }

     private:
      const std::string name_;
      T value_;
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_DATUM_H_
