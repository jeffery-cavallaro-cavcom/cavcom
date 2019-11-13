#include "csv_hit_counter_fields.h"

namespace cavcom {
  namespace utility {

    CSVHitCounterFields::CSVHitCounterFields(const std::string &name)
      : datums_(name), tries_(&datums_.tries_), hits_(&datums_.hits_), percent_(&datums_.percent_) {}

    void CSVHitCounterFields::add_fields(CSVFile *csv) {
      csv->add_field(&tries_);
      csv->add_field(&hits_);
      csv->add_field(&percent_);
    }

    void CSVHitCounterFields::add_data(const Counter::value_type &tries, const Counter::value_type &hits) {
      datums_.add(tries, hits);
    }

  }  // namespace utility
}  // namespace cavcom
