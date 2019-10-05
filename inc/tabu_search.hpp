#ifndef TS_HPP
#define TS_HPP

#include "utils.hpp"

class TabuSearchSolution : public Solution{
  bool outer_tabu=false, inner_tabu = false;
  int n_tabu = 50;
  int max_it = 2000, c_it = 0;
  int cur, prev, next_c, rep, next_r, best_c, best_r;
  double cost = 0, best_cost, new_cost;
  double delta, cost_reduction, cost_increase, bcr, bci;
  std::vector<Vehicle> best_vehicles;
  Vehicle *v_temp_2 = nullptr, *v_temp = nullptr;
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
  inline void CreateFirstArcSetToCheck(Vehicle& v, const int& cur);
  inline void CreateSecondArcSetToCheck(Vehicle& v2, const int& cur);

};

#endif TS_HPP
