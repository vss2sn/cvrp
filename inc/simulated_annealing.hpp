/**
* @file simulated_annealing.hpp
* @author vss2sn
* @brief Contains the SimulatedAnnealingSolution class
*/

#ifndef SA_HPP
#define SA_HPP

#include "utils.hpp"

class SimulatedAnnealingSolution : public Solution{
  double temp = 0, cooling_rate = 0.9999, best_cost, current_cost;
  int max_temp = 5000, n_reheates = 20;
public:
  SimulatedAnnealingSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){} ;
  SimulatedAnnealingSolution(Problem p)
    :Solution(p.nodes, p.vehicles, p.distanceMatrix){};
  void Solve();
  inline bool AllowMove(double delta);
};

#endif SA_HPP
