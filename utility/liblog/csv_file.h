#ifndef CAVCOM_UTILITY_LIBLOG_CSV_FILE_H_
#define CAVCOM_UTILITY_LIBLOG_CSV_FILE_H_

#include <vector>

#include "csv_field.h"

namespace cavcom {
  namespace utility {
    // Implements a comma-separated values (CSV) file.  A CSV file is backed by a collection of CSV fields, each of
    // which is backed by a datum.
    class CSVFile : public OutFile {
     public:
      // Creates a new output file instance.  The actual file will not be created/opened until an explicit open call.
      explicit CSVFile(const std::string &name);

      // Appends a field to the CSV field set.  The field definition (and it backing datum) must remain valid for the
      // life of the CSV file instance.
      void add_field(CSVField *field) {
        fields_.push_back(field);
      }

      // Truncates the output file and adds the header line.  The file is left open.
      void write_header(void);

      // Writes the current data values to the file.
      void write_data(void);

      // Resets the current data values.
      void reset_data(void);

     private:
      using CSVFields = std::vector<CSVField *>;
      CSVFields fields_;
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_CSV_FILE_H_
