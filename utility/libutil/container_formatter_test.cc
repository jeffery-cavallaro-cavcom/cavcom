// Exercises the container formatter.

#include <sstream>
#include <vector>

#include <libunittest/all.hpp>

#include "container_formatter.h"

using namespace cavcom::utility;

static const std::vector<int> VALUES = {10, 0, -5, 4, 100, -25, 42};

TEST(default_style) {
  std::ostringstream out;
  ContainerFormatter formatter(&out);
  formatter(VALUES.cbegin(), VALUES.cend());
  std::string expected = "{10, 0, -5, 4, 100, -25, 42}";
  UNITTEST_ASSERT_EQUAL(out.str(), expected);
}

TEST(custom_style) {
  std::ostringstream out;
  ContainerFormatter formatter(&out);
  formatter.start("[");
  formatter.end("]");
  formatter.separator("|");
  formatter(VALUES.cbegin(), VALUES.cend());
  std::string expected = "[10|0|-5|4|100|-25|42]";
  UNITTEST_ASSERT_EQUAL(out.str(), expected);
}

TEST(no_style) {
  std::ostringstream out;
  ContainerFormatter formatter(&out);
  formatter.start("");
  formatter.end("");
  formatter.separator("");
  formatter(VALUES.cbegin(), VALUES.cend());
  std::string expected = "100-54100-2542";
  UNITTEST_ASSERT_EQUAL(out.str(), expected);
}
