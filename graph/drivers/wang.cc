// Executes the Wang algorithm on a sequence of random graphs for a given order and edge probability.

#include <sstream>

#include "csv_sample_fields.h"
#include "csv_file.h"
#include "random_graph.h"
#include "chromatic_wang.h"

using namespace cavcom::utility;
using namespace cavcom::graph;

class Statistics {
 public:
  Statistics(void) : order("n"), eprob("p"), time("time"), calls("calls") {}
  CSVDatumField<VertexNumber> order;
  CSVDatumField<uint> eprob;
  CSVSampleFields<double> time;
  CSVSampleFields<ullong> calls;

  void add_fields(CSVFile *csv) {
    csv->add_field(&order);
    csv->add_field(&eprob);
    time.add_fields(csv);
    calls.add_fields(csv);
  }

  void gather_stats(VertexNumber n, uint p, const ChromaticWang &cw) {
    order.datum().value(n);
    eprob.datum().value(p);
    std::chrono::duration<double> dt = cw.duration();
    time.add_data(dt.count());
    calls.add_data(cw.calls());
  }
};

static std::string make_summary_filename(VertexNumber n) {
  std::ostringstream name;
  name << "data/summary_" << n << ".dat";
  return name.str();
}

static std::string make_raw_filename(VertexNumber n, uint ipct) {
  std::ostringstream name;
  name << "data/raw_" << n << "_" << ipct << ".dat";
  return name.str();
}

static constexpr uint TRIALS = 1000;

static constexpr VertexNumber N_START = 5;
static constexpr VertexNumber N_END = 30;
static constexpr VertexNumber N_INCR = 1;

static constexpr uint P_START = 10;
static constexpr uint P_END = 90;
static constexpr uint P_INCR = 10;

int main(int argc, char *argv[]) {
  for (VertexNumber n = N_START; n <= N_END; n += N_INCR) {
    Statistics summary_data;
    CSVFile summary_file(make_summary_filename(n));
    summary_data.add_fields(&summary_file);
    summary_file.open(true);
    summary_file.write_header();
    summary_file.close();

    for (uint ipct = P_START; ipct <= P_END; ipct += P_INCR) {
      summary_file.reset_data();

      Statistics raw_data;
      CSVFile raw_file(make_raw_filename(n, ipct));
      raw_data.add_fields(&raw_file);
      raw_file.open(true);
      raw_file.write_header();
      raw_file.close();

      uint ntrials = (n < 20) ? TRIALS : TRIALS/10;
      for (uint itrial = 0; itrial < ntrials; ++itrial) {
        raw_file.reset_data();
        RandomGraph rg(n, ipct/100.0);
        ChromaticWang cw(rg);
        cw.execute();
        raw_data.gather_stats(n, ipct, cw);
        summary_data.gather_stats(n, ipct, cw);
        raw_file.write_data();
        raw_file.close();
      }

      summary_file.write_data();
      summary_file.close();
    }
  }
}
