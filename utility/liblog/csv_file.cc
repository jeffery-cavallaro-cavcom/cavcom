#include "csv_file.h"

namespace cavcom {
  namespace utility {

    CSVFile::CSVFile(const std::string &name) : OutFile(name) {}

    void CSVFile::write_header(void) {
      close();
      open(true);
      for (CSVFields::size_type i = 0; i < fields_.size(); ++i) {
        if (i > 0) (*this) << ",";
        fields_[i]->add_header(this);
      }
      (*this) << std::endl;
    }

    void CSVFile::write_data(void) {
      for (CSVFields::size_type i = 0; i < fields_.size(); ++i) {
        if (i > 0) (*this) << ",";
        fields_[i]->add_data(this);
      }
      (*this) << std::endl;
    }

    void CSVFile::reset_data(void) {
      for (auto ifield = fields_.begin(); ifield != fields_.end(); ++ifield) {
        (*ifield)->reset_value();
      }
    }

  }  // namespace utility
}  // namespace cavcom
