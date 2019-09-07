#include "utils.hpp"

class LocalSearchIntraSolution : public Solution{
public:
  LocalSearchIntraSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){} ;
  void Solve();
};
