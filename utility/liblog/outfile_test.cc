// Creates and writes to an output file.

#include <filesystem>
#include <string>

#include <libunittest/all.hpp>

#include "outfile.h"

using namespace cavcom::utility;

static const std::filesystem::path PATH = "/tmp/test.txt";
static const std::string DATA = "Hello, World!!!";

static std::string make_path() {
  std::filesystem::path path = std::filesystem::temp_directory_path();
  path /= PATH;
  return path;
}

TEST(create_outfile) {
  OutFile out(make_path());
  UNITTEST_ASSERT_FALSE(out.is_open());
  UNITTEST_ASSERT_TRUE(out.open(true));
  UNITTEST_ASSERT_TRUE(out.is_open());
  out.close();
  UNITTEST_ASSERT_FALSE(out.is_open());
  UNITTEST_ASSERT_EQUAL(std::filesystem::file_size(out.name()), 0);
  std::filesystem::remove(out.name());
}

TEST(write_outfile) {
  OutFile out(make_path());
  UNITTEST_ASSERT_TRUE(out.open(true));
  out << DATA << std::endl;
  out.close();
  UNITTEST_ASSERT_EQUAL(std::filesystem::file_size(out.name()), DATA.size() + 1);
  std::filesystem::remove(out.name());
}

TEST(write_and_close) {
  OutFile out(make_path());
  out << DATA << std::endl;
  out.close();
  UNITTEST_ASSERT_EQUAL(std::filesystem::file_size(out.name()), DATA.size() + 1);

  out << DATA << std::endl;
  out.close();
  UNITTEST_ASSERT_EQUAL(std::filesystem::file_size(out.name()), 2*(DATA.size() + 1));

  std::filesystem::remove(out.name());
}

TEST(write_and_truncate) {
  OutFile out(make_path());
  out << DATA << std::endl;
  out.close();
  UNITTEST_ASSERT_EQUAL(std::filesystem::file_size(out.name()), DATA.size() + 1);

  UNITTEST_ASSERT_TRUE(out.open(true));
  out << DATA << std::endl;
  out.close();
  UNITTEST_ASSERT_EQUAL(std::filesystem::file_size(out.name()), DATA.size() + 1);

  std::filesystem::remove(out.name());
}
