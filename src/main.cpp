#include "main.hpp"

int main(){
  Problem p(100,4,25,16,100, "cluster");

  std::cout << "Greedy: " << std::endl;
  GreedySolution vrp_greedy(p);
  vrp_greedy.Solve();
  std::cout << std::endl;

  std::cout << "Local Search (Within each vehicle separately): " << std::endl;
  LocalSearchIntraSolution vrp_lsi(p);
  vrp_lsi.Solve();
  std::cout << std::endl;

  std::cout << "Local Search (Within all vehicles): " << std::endl;
  LocalSearchInterIntraSolution vrp_lsii(p);
  vrp_lsii.Solve();
  std::cout << std::endl;

  std::cout << "Tabu Search: " << std::endl;
  TabuSearchSolution vrp_ts(p);
  vrp_ts.Solve();
  std::cout << std::endl;

  std::cout << "Genetic Algorithm: " << std::endl;
  GASolution vrp_ga(p, 100, 5000);
  vrp_ga.Solve();
  std::cout << std::endl;

  return 0;
}
