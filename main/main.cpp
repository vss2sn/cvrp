/**
 * @file main.cpp
 * @author vss2sn
 * @brief The main function that sets up the problem and runs the solution
 * algorithms
 */

#include <iostream>

#include "cvrp/genetic_algorithm.hpp"
#include "cvrp/greedy.hpp"
#include "cvrp/local_search_inter_intra.hpp"
#include "cvrp/local_search_intra.hpp"
#include "cvrp/simulated_annealing.hpp"
#include "cvrp/tabu_search.hpp"

int main(int /* argc */, char** /* argv[] */) {
  constexpr int noc = 10;
  constexpr int demand_range = 4;
  constexpr int nov = 8;
  constexpr int capacity = 5;
  constexpr int grid_range = 10;
  Problem p(noc, demand_range, nov, capacity, grid_range, "uniform");

  std::cout << "Greedy: " << '\n';
  GreedySolution vrp_greedy(p.nodes_, p.vehicles_, p.distanceMatrix_);
  vrp_greedy.Solve();
  std::cout << '\n';

  std::cout << "Local Search (Within each vehicle separately): " << '\n';
  LocalSearchIntraSolution vrp_lsi(p);
  vrp_lsi.Solve();
  std::cout << '\n';

  std::cout << "Local Search (Within all vehicles): " << '\n';
  LocalSearchInterIntraSolution vrp_lsii(p);
  vrp_lsii.Solve();
  std::cout << '\n';

  std::cout << "Tabu Search: " << '\n';
  constexpr int n_tabu = 10;
  constexpr int max_it = 500;
  TabuSearchSolution vrp_ts(p, n_tabu, max_it);
  vrp_ts.Solve();
  std::cout << '\n';

  std::cout << "Genetic Algorithm: " << '\n';
  constexpr int n_chromosomes = 5;
  constexpr int generations = 100;
  GASolution vrp_ga(p, n_chromosomes, generations);
  vrp_ga.Solve();
  std::cout << '\n';

  std::cout << "Simulated Annealing: " << '\n';
  constexpr int stag_limit = 500000;
  constexpr double init_temp = 5000;
  constexpr double cooling_rate = 0.9999;
  constexpr int n_reheats = 20;
  SimulatedAnnealingSolution vrp_sa(p, stag_limit, init_temp, cooling_rate,
                                    n_reheats);
  vrp_sa.Solve();
  std::cout << '\n';

  // NOTE: The following are examples of how to create solutions by using the
  // algorithms coded in this repository sequentially, effectively allowing
  // the creation of hybrid algorithms/solutions.

  // Example 1
  // std::cout << "Hybrid Example 1: " << '\n';
  // GreedySolution vrp_greedy_for_hybrid(p);
  // vrp_greedy_for_hybrid.Solve();
  // Solution s = vrp_greedy_for_hybrid; // Strip out solution from
  // vrp_greedy_for_hybrid. LocalSearchInterIntraSolution
  // vrp_lsii_for_hybrid(s); vrp_lsii_for_hybrid.Solve(); std::cout << '\n';

  // Example 2
  // std::cout << "Hybrid Example 2: " << '\n';
  // GASolution vrp_ga_for_hybrid(p, 25, 500000);;
  // vrp_ga_for_hybrid.Solve();
  // Solution s = vrp_ga_for_hybrid; // Strip out solution from
  // vrp_greedy_for_hybrid. LocalSearchInterIntraSolution
  // vrp_lsii_for_hybrid(s); vrp_lsii_for_hybrid.Solve(); std::cout << '\n';

  return 0;
}
