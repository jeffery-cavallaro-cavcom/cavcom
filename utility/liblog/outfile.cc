#include "outfile.h"

namespace cavcom {
  namespace utility {

    OutFile::OutFile(const std::string &name) : name_(name) {}

    bool OutFile::open(bool truncate) {
      if (truncate) close();
      if (!is_open()) {
        std::ofstream::openmode mode = std::ofstream::out;
        mode |= (truncate ? std::ofstream::trunc : std::ofstream::app);
        out_.open(name_, mode);
      }
      return out_.good();
    }

    void OutFile::close(void) {
      if (is_open()) out_.close();
    }

  }  // namespace utility
}  // namespace cavcom
