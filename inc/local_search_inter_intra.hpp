#include "utils.hpp"

class LocalSearchInterIntraSolution : public Solution{
public:
  LocalSearchInterIntraSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){} ;
  LocalSearchInterIntraSolution(Problem p)
    :Solution(p.nodes, p.vehicles, p.distanceMatrix){};
  void Solve();
};
