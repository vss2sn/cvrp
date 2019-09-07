#include "utils.hpp"

class GreedySolution:public Solution{
public:
  GreedySolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
    :Solution(nodes, vehicles, distanceMatrix){};
  void Solve();
};
