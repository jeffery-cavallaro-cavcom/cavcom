// Exercises lookup tables with integer and string keys and values.

#include <string>

#include <libunittest/all.hpp>

#include "lookup.h"

using StringToInt = cavcom::utility::Lookup<std::string, int>;
using IntToString = cavcom::utility::Lookup<int, std::string>;

TEST(create_notfound_exception) {
  const std::string name = "stuff";
  const std::string sval = "hello";
  const int ival = 100;
  const std::string smsg = "Unknown stuff: hello";
  const std::string imsg = "Unknown stuff: 100";

  StringToInt::NotFoundLookupError es(name, sval);
  UNITTEST_ASSERT_EQUAL(es.key(), sval);
  UNITTEST_ASSERT_EQUAL(es.what(), smsg);

  IntToString::NotFoundLookupError ei(name, ival);
  UNITTEST_ASSERT_EQUAL(ei.key(), ival);
  UNITTEST_ASSERT_EQUAL(ei.what(), imsg);
}

TEST(create_duplicate_exception) {
  const std::string name = "junk";
  const std::string sval = "world";
  const int ival = 42;
  const std::string smsg = "Duplicate junk: world";
  const std::string imsg = "Duplicate junk: 42";

  StringToInt::DuplicateLookupError es(name, sval);
  UNITTEST_ASSERT_EQUAL(es.key(), sval);
  UNITTEST_ASSERT_EQUAL(es.what(), smsg);

  IntToString::DuplicateLookupError ei(name, ival);
  UNITTEST_ASSERT_EQUAL(ei.key(), ival);
  UNITTEST_ASSERT_EQUAL(ei.what(), imsg);
}

TEST(create_empty_table) {
  const std::string name = "one";

  StringToInt stoi(name);
  IntToString itos(name);

  UNITTEST_ASSERT_EQUAL(stoi.size(), 0);
  int ivalue = 0;
  UNITTEST_ASSERT_FALSE(stoi.find("hello", &ivalue));
  UNITTEST_ASSERT_THROW(StringToInt::NotFoundLookupError, [&](){ stoi.find("world", &ivalue, true); });

  UNITTEST_ASSERT_EQUAL(itos.size(), 0);
  std::string svalue;
  UNITTEST_ASSERT_FALSE(itos.find(10, &svalue));
  UNITTEST_ASSERT_THROW(IntToString::NotFoundLookupError, [&](){ itos.find(-10, &svalue, true); });
}
