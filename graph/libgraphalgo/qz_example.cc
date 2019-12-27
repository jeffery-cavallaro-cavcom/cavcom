#include <iostream>

#include "mycielski.h"
#include "quick_zykov.h"
#include "random_graph.h"

using namespace cavcom::graph;

int main(int argc, char *argv[]) {
  TikzFormatter formatter(&std::cout);
  Mycielski g(4);
  QuickZykov qz(g);
  //  QuickZykov qz(g, &formatter);
  qz.execute();
  qz.apply(&g);
  std::cout << "Mycielski: " << g.proper() << std::endl;

  while (true) {
    RandomGraph g1(10, 0.5);
    SimpleGraph g2(g1);
    bool ok = true;
    try {
      QuickZykov qz1(g1);
      qz1.execute();
      qz1.apply(&g1);
    }
    catch (const std::exception &e) {
      std::cout << "Exception: " << e.what() << std::endl;
      ok = false;
    }

    if ((!ok) || (!g1.proper())) {
      TikzFormatter f(&std::cout);
      QuickZykov qz2(g2, &f);
      qz2.execute();
      return 1;
    }
  }

  return 0;
}
