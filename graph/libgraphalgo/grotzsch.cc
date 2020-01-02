#include <iostream>

#include "mycielski.h"
#include "quick_zykov.h"
#include "chromatic_wang.h"
#include "zykov.h"
#include "bron2.h"
#include "greedy_coloring.h"

using namespace cavcom::graph;

static const Color K = 4;

int main(int argc, char *argv[]) {
  TikzFormatter formatter(&std::cout);
  Mycielski g(K);
  std::cout << "n=" << g.order() << " m=" << g.size() << std::endl;

  Bron2 b(g);
  b.execute();
  std::chrono::duration<double> bdt = b.duration();
  GreedyColoring s(g);
  s.execute();
  std::chrono::duration<double> sdt = b.duration();
  std::cout << "b+g time=" << bdt.count() + sdt.count() << std::endl;

  QuickZykov qz(g);
  //  QuickZykov qz(g, &formatter);
  qz.execute();
  std::chrono::duration<double> qzdt = qz.duration();
  std::cout << "qz: calls=" << qz.calls() << " time=" << qzdt.count() << std::endl;

  ChromaticWang cw(g);
  cw.execute();
  std::chrono::duration<double> cwdt = cw.duration();
  std::cout << "cw: calls=" << cw.calls() << " time=" << cwdt.count() << std::endl;

  Zykov z(g);
  z.execute();
  std::chrono::duration<double> zdt = z.duration();
  std::cout << "z: calls=" << z.calls() << " time=" << zdt.count() << std::endl;

  return 0;
}
