// Creates and writes to a CSV file.

#include <fstream>
#include <filesystem>

#include <libunittest/all.hpp>

#include "sum.h"
#include "percent.h"
#include "csv_hit_counter_fields.h"
#include "csv_sample_fields.h"
#include "csv_file.h"

using namespace cavcom::utility;

static const std::filesystem::path PATH = "/tmp/test.csv";

struct Fields {
  Fields(void) : letter("letter"), sum("sum"), count("count"), percent("percent"),
                 event("event"), sample("sample") {}

  CSVDatumField<char> letter;
  CSVSumField<double> sum;
  CSVCounterField count;
  CSVPercentField percent;
  CSVHitCounterFields event;
  CSVSampleFields<uint> sample;

  void set_data(char c_value, double sum_value, uint count_value, double percent_value,
                Counter::value_type tries, Counter::value_type hits, uint data) {
    letter.datum().value(c_value);
    sum.datum().add(sum_value);
    count.datum().add(count_value);
    percent.datum().value(percent_value);
    event.add_data(tries, hits);
    sample.add_data(data);
  }
};

static Fields fields;

static void add_fields(CSVFile *csv) {
  csv->add_field(&fields.letter);
  csv->add_field(&fields.sum);
  csv->add_field(&fields.count);
  csv->add_field(&fields.percent);
  fields.event.add_fields(csv);
  fields.sample.add_fields(csv);
}

static std::string make_path() {
  std::filesystem::path path = std::filesystem::temp_directory_path();
  path /= PATH;
  return path;
}

static const std::string EXPECTED =
  "letter,sum,count,percent,"
  "event_tries,event_hits,event_pct,"
  "sample_total,sample_count,sample_mean,sample_min,sample_max\n"
  "A,2.5,1,50,5,0,0,4,1,4,4,4\n"
  "B,3.75,4,56.8,9,3,33.3,4,2,2,0,4\n";

TEST(create_csv_file) {
  CSVFile csv(make_path());
  add_fields(&csv);
  csv.write_header();

  fields.set_data('A', 2.5, 1, 0.5, 5, 0, 4);
  csv.write_data();

  fields.set_data('B', 1.25, 3, 0.56789, 4, 3, 0);
  csv.write_data();

  csv.close();

  UNITTEST_ASSERT_EQUAL(std::filesystem::file_size(PATH), EXPECTED.size());

  std::string contents;
  std::string next;
  std::ifstream in;
  in.open(PATH);
  while (std::getline(in, next)) {
    contents += next;
    contents += "\n";
  }
  in.close();
  UNITTEST_ASSERT_EQUAL(contents, EXPECTED);

  std::filesystem::remove(PATH);
}
