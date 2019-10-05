#ifndef TS_HPP
#define TS_HPP

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
  bool outer_tabu=false, inner_tabu = false;

  double delta, cost_reduction, cost_increase, bcr, bci;
  int cur, prev, next_c, rep, next_r, best_c, best_r;
  Vehicle *v_temp_2, *v_temp;

  int n_tabu = 50;
  int max_it = 2000, c_it = 0;
  std::vector<Vehicle> best_vehicles;
  double cost = 0, best_cost, new_cost;
  std::vector<std::vector<int>> to_check = std::vector<std::vector<int>>(6,std::vector<int>(2,0));
  std::unordered_set<std::vector<int>, VectorHash> tabu_list_set;
  std::queue<std::vector<int>> tabu_list_queue;
  std::unordered_set<std::vector<int>, VectorHash>::iterator tabu_list_set_end;
public:
  TabuSearchSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){} ;
  TabuSearchSolution(Problem p)
    :Solution(p.nodes, p.vehicles, p.distanceMatrix){};
  void Solve();
  inline bool CheckTabu(int begin, int end);

};

#endif TS_HPP
