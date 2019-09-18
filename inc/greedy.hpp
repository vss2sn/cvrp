#ifndef GREEDY_HPP
#define GREEDY_HPP

#include "utils.hpp"

class GreedySolution:public Solution{
public:
  GreedySolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
    :Solution(nodes, vehicles, distanceMatrix){};
    GreedySolution(Problem p)
      :Solution(p.nodes, p.vehicles, p.distanceMatrix){};
  void Solve();
};
#endif