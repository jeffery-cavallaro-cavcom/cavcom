#ifndef CAVCOM_UTILITY_LIBLOG_CSV_SAMPLE_FIELDS_H_
#define CAVCOM_UTILITY_LIBLOG_CSV_SAMPLE_FIELDS_H_

#include "sample.h"
#include "csv_file.h"

namespace cavcom {
  namespace utility {
    // Provides CSV field bindings for all of the datums in a hit counter.
    template <typename T> class CSVSampleFields {
     public:
      // Creates a new sample CSV field set with backing datums.
      CSVSampleFields(const std::string &name)
        : datums_(name),
          total_(&datums_.total_),
          count_(&datums_.count_),
          mean_(&datums_.mean_),
          min_(&datums_.min_),
          max_(&datums_.max_) {}

      // Appends the sample CSV fields to a CSV file definition.
      void add_fields(CSVFile *csv) {
        csv->add_field(&total_);
        csv->add_field(&count_);
        csv->add_field(&mean_);
        csv->add_field(&min_);
        csv->add_field(&max_);
      }

      // Adds a sample.
      void add_data(const T &sample) { datums_.add(sample); }

     private:
      Sample<T> datums_;

      CSVDatumByPtr<Sum<T>> total_;
      CSVDatumByPtr<Counter> count_;
      CSVDatumByPtr<Datum<double>> mean_;
      CSVDatumByPtr<Datum<T>> min_;
      CSVDatumByPtr<Datum<T>> max_;
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_CSV_SAMPLE_FIELDS_H_
