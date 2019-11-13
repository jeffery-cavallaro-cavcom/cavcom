#ifndef CAVCOM_UTILITY_LIBLOG_CSV_HIT_COUNTER_FIELDS_H_
#define CAVCOM_UTILITY_LIBLOG_CSV_HIT_COUNTER_FIELDS_H_

#include "hit_counter.h"
#include "csv_file.h"

namespace cavcom {
  namespace utility {
    // Provides CSV field bindings for all of the datums in a hit counter.
    class CSVHitCounterFields {
     public:
      // Creates a new hit counter CSV field set with backing datums.
      CSVHitCounterFields(const std::string &name);

      // Appends the hit counter CSV fields to a CSV file definition.
      void add_fields(CSVFile *csv);

      // Adds the specified amount of tries and hits and recalculates the percentage.
      void add_data(const Counter::value_type &tries, const Counter::value_type &hits);

     private:
      HitCounter datums_;

      CSVDatumByPtr<Counter> tries_;
      CSVDatumByPtr<Counter> hits_;
      CSVDatumByPtr<Percent> percent_;
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_CSV_HIT_COUNTER_FIELDS_H_
