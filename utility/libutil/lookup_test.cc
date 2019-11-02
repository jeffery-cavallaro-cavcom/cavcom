// Exercises lookup tables with integer and string keys and values.

#include <algorithm>
#include <string>
#include <vector>

#include <libunittest/all.hpp>

#include "lookup.h"

using StringToInt = cavcom::utility::Lookup<std::string, int>;

TEST(create_notfound_exception) {
  const std::string name = "stuff";
  const std::string val = "hello";
  const std::string msg = "Unknown stuff: hello";

  StringToInt::NotFoundLookupError es(name, val);
  UNITTEST_ASSERT_EQUAL(es.key(), val);
  UNITTEST_ASSERT_EQUAL(es.what(), msg);
}

TEST(create_duplicate_exception) {
  const std::string name = "junk";
  const std::string val = "world";
  const std::string msg = "Duplicate junk: world";

  StringToInt::DuplicateLookupError es(name, val);
  UNITTEST_ASSERT_EQUAL(es.key(), val);
  UNITTEST_ASSERT_EQUAL(es.what(), msg);
}

TEST(create_empty_table) {
  const std::string name = "one";

  StringToInt lookup(name);

  UNITTEST_ASSERT_EQUAL(lookup.size(), 0);
  int ivalue = 0;
  UNITTEST_ASSERT_FALSE(lookup.find("hello", &ivalue));
  UNITTEST_ASSERT_THROW(StringToInt::NotFoundLookupError, [&](){ lookup.find("world", &ivalue, true); });
}

using TestValue = struct { std::string svalue; int ivalue; };
using TestData = std::vector<TestValue>;
using SizeType = TestData::size_type;

static const TestData TESTDATA =
  {
   {"zero", 0}, {"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5},
   {"six", 6}, {"seven", 7}, {"eight", 8}, {"nine", 9}, {"ten", 10}
  };

static void add_values(StringToInt &table, SizeType first, SizeType last) {
  for (SizeType iv = first; iv < last; ++iv) {
    const TestValue &v = TESTDATA[iv];
    UNITTEST_ASSERT_TRUE(table.add(v.svalue, v.ivalue, true));
    int found;
    UNITTEST_ASSERT_TRUE(table.find(v.svalue, &found, true));
    UNITTEST_ASSERT_EQUAL(found, v.ivalue);
  }
}

TEST(add_and_lookup) {
  StringToInt lookup;
  UNITTEST_ASSERT_EQUAL(lookup.size(), 0);

  // Add the test values in two chunks.
  SizeType n = TESTDATA.size();
  SizeType n2 = n/2;
  add_values(lookup, 0, n2);
  UNITTEST_ASSERT_EQUAL(lookup.size(), n2);
  add_values(lookup, n2, n);
  UNITTEST_ASSERT_EQUAL(lookup.size(), n);

  // Access randomly.
  unittest::random_value<TestData::size_type> picker(TESTDATA.size() - 1);
  for (uint i = 0; i < 100; ++i) {
    TestData::size_type iv = picker.get();
    const TestValue &v = TESTDATA[iv];
    int found;
    UNITTEST_ASSERT_TRUE(lookup.find(v.svalue, &found, true));
    UNITTEST_ASSERT_EQUAL(found, v.ivalue);
  }
}

TEST(key_not_found) {
  const std::string BAD = "xxx";
  StringToInt lookup;
  add_values(lookup, 0, TESTDATA.size());
  int found;
  UNITTEST_ASSERT_FALSE(lookup.find(BAD, &found));
  UNITTEST_ASSERT_THROW(StringToInt::NotFoundLookupError, [&](){ lookup.find(BAD, &found, true); });
}

TEST(duplicate_key) {
  SizeType n = TESTDATA.size();
  const TestValue &v = TESTDATA[n/2];
  StringToInt lookup;
  add_values(lookup, 0, n);
  UNITTEST_ASSERT_FALSE(lookup.add(v.svalue, v.ivalue));
  UNITTEST_ASSERT_THROW(StringToInt::DuplicateLookupError, [&](){ lookup.add(v.svalue, v.ivalue, true); });
}

TEST(remove_key) {
  SizeType n = TESTDATA.size();
  const TestValue &v = TESTDATA[n/2];
  StringToInt lookup;
  add_values(lookup, 0, n);
  lookup.remove(v.svalue);
  UNITTEST_ASSERT_EQUAL(lookup.size(), n - 1);
  int found;
  UNITTEST_ASSERT_FALSE(lookup.find(v.svalue, &found));
  UNITTEST_ASSERT_TRUE(lookup.add(v.svalue, v.ivalue, true));
  UNITTEST_ASSERT_EQUAL(lookup.size(), n);
  UNITTEST_ASSERT_TRUE(lookup.find(v.svalue, &found, true));
}

TEST(clear_table) {
  SizeType n = TESTDATA.size();
  StringToInt lookup;
  add_values(lookup, 0, n);
  lookup.clear();
  UNITTEST_ASSERT_EQUAL(lookup.size(), 0);
  for (SizeType iv = 0; iv < n; ++iv) {
    const TestValue &v = TESTDATA[iv];
    int found;
    UNITTEST_ASSERT_FALSE(lookup.find(v.svalue, &found));
  }
  add_values(lookup, 0, n);
  UNITTEST_ASSERT_EQUAL(lookup.size(), n);
}
