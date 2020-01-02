#include <iostream>

#include "mycielski.h"
#include "quick_zykov.h"
#include "chromatic_wang.h"
#include "zykov.h"
#include "bron2.h"
#include "greedy_coloring.h"

using namespace cavcom::utility;
using namespace cavcom::graph;

static const Color K = 4;
static const Color NTRIALS = 100;

int main(int argc, char *argv[]) {
  TikzFormatter formatter(&std::cout);
  Mycielski g(K);
  std::cout << "n=" << g.order() << " m=" << g.size() << std::endl;

  double bg_time = 0.0;
  double qz_time = 0.0;
  ullong qz_calls = 0;
  double cw_time = 0.0;
  ullong cw_calls = 0;
  double z_time = 0.0;
  ullong z_calls = 0;

  for (uint itrial = 0; itrial < NTRIALS; ++itrial) {
    Bron2 b(g);
    b.execute();
    std::chrono::duration<double> bdt = b.duration();
    GreedyColoring s(g);
    s.execute();
    std::chrono::duration<double> sdt = b.duration();
    bg_time += (bdt.count() + sdt.count());
  }
  std::cout << "b+g time=" << bg_time/NTRIALS << std::endl;

  for (uint itrial = 0; itrial < NTRIALS; ++itrial) {
    QuickZykov qz(g);
    qz.execute();
    std::chrono::duration<double> dt = qz.duration();
    qz_time += dt.count();
    qz_calls += qz.calls();
  }
  std::cout << "qz: calls=" << qz_calls/NTRIALS << " time=" << qz_time/NTRIALS << std::endl;

  for (uint itrial = 0; itrial < NTRIALS; ++itrial) {
    ChromaticWang cw(g);
    cw.execute();
    std::chrono::duration<double> dt = cw.duration();
    cw_time += dt.count();
    cw_calls += cw.calls();
  }
  std::cout << "cw: calls=" << cw_calls/NTRIALS << " time=" << cw_time/NTRIALS << std::endl;

  for (uint itrial = 0; itrial < NTRIALS; ++itrial) {
    Zykov z(g);
    z.execute();
    std::chrono::duration<double> dt = z.duration();
    z_time += dt.count();
    z_calls += z.calls();
  }
  std::cout << "z: calls=" << z_calls/NTRIALS << " time=" << z_time/NTRIALS << std::endl;

  return 0;
}
