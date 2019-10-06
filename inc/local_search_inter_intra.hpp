/**
* @file local_search_inter_intra.hpp
* @author vss2sn
* @brief Contains the LocalSearchInterIntraSolution class (Local search extends to all vehicles)
*/

#ifndef LSII_HPP
#define LSII_HPP

#include "utils.hpp"

class LocalSearchInterIntraSolution : public Solution{
public:
  LocalSearchInterIntraSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){} ;
  LocalSearchInterIntraSolution(Problem p)
    :Solution(p.nodes, p.vehicles, p.distanceMatrix){};
  void Solve();
};

#endif LSII_HPP
