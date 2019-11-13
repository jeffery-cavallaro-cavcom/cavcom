// Executes the quick Zykov algorithm on a sequence of random graphs for a given order and edge probability.

#include <sstream>

#include "csv_sample_fields.h"
#include "csv_hit_counter_fields.h"
#include "csv_file.h"
#include "random_graph.h"
#include "quick_zykov.h"

using namespace cavcom::utility;
using namespace cavcom::graph;

class Statistics {
 public:
  Statistics(void) : order("n"), eprob("p"),
                     time("time"), steps("steps"), calls("calls"), depth("depth"),
                     edge_threshold("edge_threshold"),
                     small_degree("small_degree"),
                     neighborhood_subset("neighborhood_subset"),
                     common_neighbors("common_neighbors") {}
  CSVDatumField<VertexNumber> order;
  CSVDatumField<uint> eprob;
  CSVSampleFields<double> time;
  CSVSampleFields<ullong> steps;
  CSVSampleFields<ullong> calls;
  CSVSampleFields<ullong> depth;
  CSVHitCounterFields edge_threshold;
  CSVHitCounterFields small_degree;
  CSVHitCounterFields neighborhood_subset;
  CSVHitCounterFields common_neighbors;

  void add_fields(CSVFile *csv) {
    csv->add_field(&order);
    csv->add_field(&eprob);
    time.add_fields(csv);
    steps.add_fields(csv);
    calls.add_fields(csv);
    depth.add_fields(csv);
    edge_threshold.add_fields(csv);
    small_degree.add_fields(csv);
    neighborhood_subset.add_fields(csv);
    common_neighbors.add_fields(csv);
  }

  void gather_stats(VertexNumber n, uint p, const QuickZykov &qz) {
    order.datum().value(n);
    eprob.datum().value(p);
    std::chrono::duration<double> dt = qz.duration();
    time.add_data(dt.count());
    steps.add_data(qz.steps());
    calls.add_data(qz.calls());
    depth.add_data(qz.maxdepth());
    edge_threshold.add_data(qz.edge_threshold_tries(), qz.edge_threshold_hits());
    small_degree.add_data(qz.small_degree_tries(), qz.small_degree_hits());
    neighborhood_subset.add_data(qz.neighborhood_subset_tries(), qz.neighborhood_subset_hits());
    common_neighbors.add_data(qz.common_neighbors_tries(), qz.common_neighbors_hits());
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
static constexpr VertexNumber N_END = 20;
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

      for (uint itrial = 0; itrial < TRIALS; ++itrial) {
        raw_file.reset_data();
        RandomGraph rg(n, ipct/100.0);
        QuickZykov qz(rg);
        qz.execute();
        raw_data.gather_stats(n, ipct, qz);
        summary_data.gather_stats(n, ipct, qz);
        raw_file.write_data();
        raw_file.close();
      }

      summary_file.write_data();
      summary_file.close();
    }
  }

#if 0
    if ((itrial > 0) && ((itrial + 1) % 10) == 0) {
      std::cout << "Completed: " << itrial + 1 << " in " << t.sum() << std::endl;
    }
  }
#endif
}
