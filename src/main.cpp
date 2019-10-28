/**
* @file main.cpp
* @author vss2sn
* @brief The main function that sets up the problem and runs the solution algorithms
*/

#include "main.hpp"

int main(){
  Problem p(10,4,8,5,10, "uniform");

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
  TabuSearchSolution vrp_ts(p, 10);
  vrp_ts.Solve();
  std::cout << std::endl;

  std::cout << "Genetic Algorithm: " << std::endl;
  GASolution vrp_ga(p, 25, 50000);
  vrp_ga.Solve();
  std::cout << std::endl;

  std::cout << "Simulated Annealing: " << std::endl;
  SimulatedAnnealingSolution vrp_sa(p, 5000000, 5000, 0.9999);
  vrp_sa.Solve();
  std::cout << std::endl;

  // NOTE: The following is an example of how to create solutions by using the
  // algorithms coding in this repository sequentially, effectively allowing
  // the creation of hybrid algorithms/solutions
  //
  // GreedySolution vrp_greedy(p);
  // vrp_greedy.Solve();
  // Solution s = vrp_greedy; // Strip out solution from vrp_greedy.
  // LocalSearchInterIntraSolution hybrid(s);
  // hybrid.Solve();
  // std::cout << std::endl;

  return 0;
}
