#ifndef CAVCOM_UTILITY_LIBLOG_CSV_FIELD_H_
#define CAVCOM_UTILITY_LIBLOG_CSV_FIELD_H_

#include "datum.h"
#include "sum.h"
#include "percent.h"
#include "out_file.h"

namespace cavcom {
  namespace utility {
    // The base class for a single field in a CSV file.  Derived classes add the datum semantics.
    class CSVField {
     public:
      virtual void add_header(OutFile *out) const = 0;
      virtual void add_data(OutFile *out) const = 0;
      virtual void reset_value(void) = 0;
    };

    // A template to bind the datum semantics to the base CSV field by composition.
    template <typename T> class CSVDatum : public CSVField {
     public:
      CSVDatum(const std::string &name) : datum_(name) {}

      T &datum(void) { return datum_; }

      virtual void add_header(OutFile *out) const {
        *out << datum_.name();
      }

      virtual void add_data(OutFile *out) const {
        *out << datum_.value();
      }

      virtual void reset_value(void) { datum_.reset(); }

     private:
      T datum_;
    };

    // A template to bind the datum semantics to the base CSV field by pointer.
    template <typename T> class CSVDatumByPtr : public CSVField {
     public:
      CSVDatumByPtr(T *datum) : datum_(datum) {}

      T &datum(void) { return *datum_; }

      virtual void add_header(OutFile *out) const {
        *out << datum_->name();
      }

      virtual void add_data(OutFile *out) const {
        *out << datum_->value();
      }

      virtual void reset_value(void) { datum_->reset(); }

     private:
      T *datum_;
    };

    // Specializations for the common datum types.
    template <typename T> class CSVDatumField : public CSVDatum<Datum<T>> {
     public:
      CSVDatumField(const std::string &name) : CSVDatum<Datum<T>>(name) {}
    };

    template <typename T> class CSVSumField : public CSVDatum<Sum<T>> {
     public:
      CSVSumField(const std::string &name) : CSVDatum<Sum<T>>(name) {}
    };

    class CSVCounterField : public CSVDatum<Counter> {
     public:
      CSVCounterField(const std::string &name) : CSVDatum<Counter>(name) {}
    };

    class CSVPercentField : public CSVDatum<Percent> {
     public:
      CSVPercentField(const std::string &name) : CSVDatum<Percent>(name) {}
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_CSV_FIELD_H_
