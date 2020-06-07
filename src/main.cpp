/**
* @file main.cpp
* @author vss2sn
* @brief The main function that sets up the problem and runs the solution algorithms
*/

#include "main.hpp"
#include <thread>

int main(int argc, char * argv[]){

  Problem p(10,4,8,5,10, "uniform");

#ifdef VISUALIZE
  rclcpp::init(argc, argv);
  mpp = std::make_shared<MinimalPublisher>();
  std::thread thread_ros = std::thread([&]{rclcpp::spin(mpp);rclcpp::shutdown();});
  thread_ros.detach();
#endif // VISUALIZE
  std::cout << "Greedy: " << std::endl;
  GreedySolution vrp_greedy(p);
  vrp_greedy.Solve();
  std::cout << std::endl;
  //
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
  GASolution vrp_ga(p, 5, 100);
  vrp_ga.Solve();
  std::cout << std::endl;
  //
  std::cout << "Simulated Annealing: " << std::endl;
  SimulatedAnnealingSolution vrp_sa(p, 5000000, 5000, 0.9999);
  vrp_sa.Solve();
  std::cout << std::endl;

  // NOTE: The following are examples of how to create solutions by using the
  // algorithms coded in this repository sequentially, effectively allowing
  // the creation of hybrid algorithms/solutions.

  // Example 1
  // std::cout << "Hybrid Example 1: " << std::endl;
  // GreedySolution vrp_greedy_for_hybrid(p);
  // vrp_greedy_for_hybrid.Solve();
  // Solution s = vrp_greedy_for_hybrid; // Strip out solution from vrp_greedy_for_hybrid.
  // LocalSearchInterIntraSolution vrp_lsii_for_hybrid(s);
  // vrp_lsii_for_hybrid.Solve();
  // std::cout << std::endl;

  // Example 2
  // std::cout << "Hybrid Example 2: " << std::endl;
  // GASolution vrp_ga_for_hybrid(p, 25, 500000);;
  // vrp_ga_for_hybrid.Solve();
  // Solution s = vrp_ga_for_hybrid; // Strip out solution from vrp_greedy_for_hybrid.
  // LocalSearchInterIntraSolution vrp_lsii_for_hybrid(s);
  // vrp_lsii_for_hybrid.Solve();
  // std::cout << std::endl;

#ifdef VISUALIZE
  rclcpp::shutdown();
#endif //VISUALIZE
  return 0;
}
