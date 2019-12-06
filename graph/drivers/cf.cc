// Executes the Christofides algorithm on a sequence of random graphs for a given order and edge probability.

#include <sstream>

#include "csv_sample_fields.h"
#include "csv_file.h"
#include "random_graph.h"
#include "christofides.h"

using namespace cavcom::utility;
using namespace cavcom::graph;

class Statistics {
 public:
  Statistics(void) : order("n"), eprob("p"), edges("m"), time("time"), calls("calls") {}
  CSVDatumField<VertexNumber> order;
  CSVDatumField<uint> eprob;
  CSVSampleFields<uint> edges;
  CSVSampleFields<double> time;
  CSVSampleFields<ullong> calls;

  void add_fields(CSVFile *csv) {
    csv->add_field(&order);
    csv->add_field(&eprob);
    edges.add_fields(csv);
    time.add_fields(csv);
    calls.add_fields(csv);
  }

  void gather_stats(VertexNumber n, uint p, const Christofides &cf) {
    order.datum().value(n);
    eprob.datum().value(p);
    edges.add_data(cf.graph().size());
    std::chrono::duration<double> dt = cf.duration();
    time.add_data(dt.count());
    calls.add_data(cf.calls());
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

static constexpr VertexNumber N_START = 17;
static constexpr VertexNumber N_END = 50;
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
        Christofides cf(rg);
        cf.execute();
        raw_data.gather_stats(n, ipct, cf);
        summary_data.gather_stats(n, ipct, cf);
        raw_file.write_data();
        raw_file.close();
      }

      summary_file.write_data();
      summary_file.close();
    }
  }
}
