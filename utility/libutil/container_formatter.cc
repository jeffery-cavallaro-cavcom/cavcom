#include <algorithm>

#include "container_formatter.h"

namespace cavcom {
  namespace utility {

    ContainerFormatter::ContainerFormatter(std::ostream *out)
      : out_(out), start_("{"), end_("}"), separator_(", ") {}

  }  // namespace name
}  // namespace cavcom
