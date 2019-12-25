#ifndef CAVCOM_UTILITY_LIBUTIL_CONTAINER_FORMATTER_H_
#define CAVCOM_UTILITY_LIBUTIL_CONTAINER_FORMATTER_H_

#include <iterator>
#include <iostream>
#include <string>

namespace cavcom {
  namespace utility {

    // A class that formats a container to an output stream.
    class ContainerFormatter {
     public:
      // Creates a new container formatter to format to the specified output stream.
      ContainerFormatter(std::ostream *out = &std::cout);

      // The character sequence output at the start of the container.  Defaults to "{".
      const std::string &start(void) const { return start_; }
      void start(const std::string &start) { start_ = start; }

      // The character sequence output at the end of the container.  Defaults to "}".
      const std::string &end(void) const { return end_; }
      void end(const std::string &end) { end_ = end; }

      // The character sequence output between elements.  Defaults to ", ".
      const std::string &separator(void) const { return separator_; }
      void separator(const std::string &separator) { separator_ = separator; }

      // Formats the specified range of a container.
      template <typename I> std::ostream &operator()(I begin, I end) {
        if (!start_.empty()) *out_ << start_;

        bool first = true;
        std::for_each(begin, end,
                      [this, &first](const typename I::value_type &value){
                        if (first) {
                          first = false;
                        } else {
                          *out_ << separator_;
                        }
                        *out_ << value;
                      });

        if (!end_.empty()) *out_ << end_;

        return *out_;
      }

     private:
      std::ostream *out_;
      std::string start_;
      std::string end_;
      std::string separator_;
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBUTIL_CONTAINER_FORMATTER_H_
