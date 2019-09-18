#ifndef TS2_HPP
#define TS2_HPP

#include "utils.hpp"

struct VectorHash {
    size_t operator()(const std::vector<int>& v) const {
        std::hash<int> hasher;
        size_t seed = 0;
        for (int i : v) {
            seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        return seed;
    }
};

class TabuSearchSolution : public Solution{
  int capacity;
  std::unordered_set<std::vector<int>, VectorHash> tabu_list_set;
  std::queue<std::vector<int>> tabu_list_queue;
public:
  TabuSearchSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){} ;
  TabuSearchSolution(Problem p)
    :Solution(p.nodes, p.vehicles, p.distanceMatrix){};
  void Solve();
};

#endif TS2_HPP
