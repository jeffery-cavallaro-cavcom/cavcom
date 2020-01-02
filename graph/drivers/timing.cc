// Executes the quick Zykov, Christofides/Wang, and Zykov algorithms of the same set of sample graphs, recording
// the runtime duration of each.  Only graphs where the lower and upper bounds don't match are considered.

#include <sstream>

#include "csv_sample_fields.h"
#include "csv_file.h"
#include "random_graph.h"
#include "quick_zykov.h"
#include "chromatic_wang.h"
#include "zykov.h"

using namespace cavcom::utility;
using namespace cavcom::graph;

class Statistics {
 public:
  Statistics(void) : order("n"), eprob("p"), quick("quick"), wang("wang"), zykov("zykov") {}
  CSVDatumField<VertexNumber> order;
  CSVDatumField<uint> eprob;
  CSVSampleFields<double> quick;
  CSVSampleFields<double> wang;
  CSVSampleFields<double> zykov;

  void add_fields(CSVFile *csv) {
    csv->add_field(&order);
    csv->add_field(&eprob);
    quick.add_fields(csv);
    wang.add_fields(csv);
    zykov.add_fields(csv);
  }

  void gather_stats(VertexNumber n, uint p, const QuickZykov &qz, const ChromaticWang &w, const Zykov &z) {
    order.datum().value(n);
    eprob.datum().value(p);
    std::chrono::duration<double> qzdt = qz.duration();
    quick.add_data(qzdt.count());
    std::chrono::duration<double> wdt = w.duration();
    wang.add_data(wdt.count());
    std::chrono::duration<double> zdt = z.duration();
    zykov.add_data(zdt.count());
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

static constexpr uint TRIALS = 100;

static constexpr VertexNumber N_START = 10;
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

      for (uint itrial = 0; itrial < TRIALS; ++itrial) {
        raw_file.reset_data();
        while (true) {
          RandomGraph rg(n, ipct/100.0);
          QuickZykov qz(rg);
          qz.execute();
          if (qz.kmin() == qz.kmax()) continue;
          ChromaticWang w(rg);
          w.execute();
          Zykov z(rg);
          z.execute();
          raw_data.gather_stats(n, ipct, qz, w, z);
          summary_data.gather_stats(n, ipct, qz, w, z);
          raw_file.write_data();
          raw_file.close();
          break;
        }
      }

      summary_file.write_data();
      summary_file.close();
    }
  }
}
