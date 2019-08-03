// Exercises the common typedefs.

#include <limits>

#include <libunittest/all.hpp>

#include "types.h"

TEST(check_type_limits) {
  UNITTEST_TESTINFO("Check limits for common typedefs");
  // tiny = signed char
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<tiny>::min(), std::numeric_limits<signed char>::min());
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<tiny>::max(), std::numeric_limits<signed char>::max());
  // utiny = unsigned char
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<utiny>::min(), std::numeric_limits<unsigned char>::min());
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<utiny>::max(), std::numeric_limits<unsigned char>::max());
  // byte = utiny
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<byte>::min(), std::numeric_limits<utiny>::min());
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<byte>::max(), std::numeric_limits<utiny>::max());
  // ushort = unsigned short
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<ushort>::min(), std::numeric_limits<unsigned short>::min());
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<ushort>::max(), std::numeric_limits<unsigned short>::max());
  // uint = unsigned int
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<uint>::min(), std::numeric_limits<unsigned int>::min());
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<uint>::max(), std::numeric_limits<unsigned int>::max());
  // ulong = unsigned long
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<ulong>::min(), std::numeric_limits<unsigned long>::min());
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<ulong>::max(), std::numeric_limits<unsigned long>::max());
  // llong = long long
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<llong>::min(), std::numeric_limits<long long>::min());
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<llong>::max(), std::numeric_limits<long long>::max());
  // ullong = unsigned long long
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<ullong>::min(), std::numeric_limits<unsigned long long>::min());
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<ullong>::max(), std::numeric_limits<unsigned long long>::max());
  // ldouble = long double
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<ldouble>::min(), std::numeric_limits<long double>::min());
  UNITTEST_ASSERT_EQUAL(std::numeric_limits<ldouble>::max(), std::numeric_limits<long double>::max());
}
