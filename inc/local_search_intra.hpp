/**
* @file local_search_intra.hpp
* @author vss2sn
* @brief Contains the LocalSearchIntraSolution class (Local search restricted to within individual vehicles)
*/

#ifndef LSI_HPP
#define LSI_HPP

#include "utils.hpp"

class LocalSearchIntraSolution : public Solution{
public:
  LocalSearchIntraSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){} ;
  LocalSearchIntraSolution(Problem p)
    :Solution(p.nodes, p.vehicles, p.distanceMatrix){};
  void Solve();
};

#endif
