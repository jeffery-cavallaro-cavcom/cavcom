#ifndef CAVCOM_UTILITY_LIBLOG_OUT_FILE_H_
#define CAVCOM_UTILITY_LIBLOG_OUT_FILE_H_

#include <fstream>
#include <string>

namespace cavcom {
  namespace utility {

    // A file used for program output.  The file is optionally truncated on open/create and is always appended.
    class OutFile {
     public:
      // Creates a new output file instance.  The actual file will not be created/opened until an explicit open call.
      explicit OutFile(const std::string &name);

      // Returns the output file name.
      const std::string &name(void) { return name_; }

      // Returns true if the file is open.
      bool is_open(void) const { return out_.is_open(); }

      // Opens the output file in append mode with optional truncation.  If the file was already open and not
      // truncating then nothing is done.  If truncating then an open file will be closed and reopened.  Returns
      // true on success.
      bool open(bool truncate = false);

      // Closes the file, if open.
      void close(void);

      // Makes sure that the output file is open and then writes the specified data to it.
      template <typename T> OutFile &operator<<(const T &data) {
        if (open()) out_ << data;
        return *this;
      }

      // Used for manipulators like endl.
      OutFile &operator<<(std::ostream &(*pf)(std::ostream &os)) {
        if (open()) pf(out_);
        return *this;
      }

     private:
      const std::string name_;
      std::ofstream out_;
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBLOG_OUT_FILE_H_
