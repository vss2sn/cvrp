/**
 * @file simulated_annealing.cpp
 * @author vss2sn
 * @brief Contains the SimulatedAnnealingSolution class
 */

#include "simulated_annealing.hpp"

SimulatedAnnealingSolution::SimulatedAnnealingSolution(
    std::vector<Node> nodes, std::vector<Vehicle> vehicles,
    std::vector<std::vector<double>> distanceMatrix, int stag_limit,
    double init_temp, double cooling_rate)
    : Solution(nodes, vehicles, distanceMatrix) {
  this->stag_limit_ = stag_limit;
  this->max_temp_ = init_temp;
  this->cooling_rate_ = cooling_rate;
  CreateInitialSolution();
}

SimulatedAnnealingSolution::SimulatedAnnealingSolution(Problem p,
                                                       int stag_limit,
                                                       double init_temp,
                                                       double cooling_rate)
    : Solution(p.nodes_, p.vehicles_, p.distanceMatrix_) {
  this->stag_limit_ = stag_limit;
  this->max_temp_ = init_temp;
  this->cooling_rate_ = cooling_rate;
  CreateInitialSolution();
}

SimulatedAnnealingSolution::SimulatedAnnealingSolution(Solution s,
                                                       int stag_limit,
                                                       double init_temp,
                                                       double cooling_rate)
    : Solution(s) {
  this->stag_limit_ = stag_limit;
  this->max_temp_ = init_temp;
  this->cooling_rate_ = cooling_rate;
  if (!s.CheckSolutionValid()) {
    std::cout << "The input solution is invalid. Exiting." << '\n';
    exit(0);
  }
}

inline bool SimulatedAnnealingSolution::AllowMove(
    double delta) { // Vehicle *v1, Vehicle *v2, int cur, int rep){
  if (delta < -0.0000000001)
    return true;
  else if (((double)rand() / RAND_MAX) < exp(-delta / temp_))
    return true;
  else
    return false;
}

void SimulatedAnnealingSolution::Solve() {
  double cost = 0;
  for (const auto &v : vehicles_)
    cost += v.cost_;
  auto best_vehicles = vehicles_;
  best_cost_ = cost;
  current_cost_ = cost;
  int n_vehicles = vehicles_.size(), cur, rep, prev, next_c, next_r;
  Vehicle *v1, *v2;
  double cost_increase, cost_reduction, delta;
  for (int r = 0; r < n_reheats_; r++) {
    // std::cout << "Reheat number: " << r << '\n';
    stag_ = stag_limit_;
    temp_ = max_temp_;
    while (--stag_ >= 0) {
      temp_ *= cooling_rate_;
      v1 = &vehicles_[rand() % n_vehicles];
      v2 = &vehicles_[rand() % n_vehicles];
      if (v1->nodes_.size() > 2)
        cur = rand() % (v1->nodes_.size() - 2) +
              1; // do not select trailing zero or starting zero
      else
        continue;
      rep = rand() % (v2->nodes_.size() - 1); // do not select trailing zero
      if (v1->id_ == v2->id_ && (cur == rep + 1 || cur == rep))
        continue;
      prev = cur - 1;
      next_c = cur + 1;
      next_r = rep + 1;
      cost_reduction = distanceMatrix_[v1->nodes_[prev]][v1->nodes_[next_c]] -
                       distanceMatrix_[v1->nodes_[prev]][v1->nodes_[cur]] -
                       distanceMatrix_[v1->nodes_[cur]][v1->nodes_[next_c]];
      cost_increase = distanceMatrix_[v2->nodes_[rep]][v1->nodes_[cur]] +
                      distanceMatrix_[v1->nodes_[cur]][v2->nodes_[next_r]] -
                      distanceMatrix_[v2->nodes_[rep]][v2->nodes_[next_r]];
      delta = cost_increase + cost_reduction;
      if ((v2->load_ - nodes_[v1->nodes_[cur]].demand_ >= 0 ||
           v1->id_ == v2->id_) &&
          AllowMove(delta)) {
        v1->load_ += nodes_[v1->nodes_[cur]].demand_;
        v2->load_ -= nodes_[v1->nodes_[cur]].demand_;
        v1->cost_ += cost_reduction;
        v2->cost_ += cost_increase;
        int val = v1->nodes_[cur];
        v1->nodes_.erase(v1->nodes_.begin() + cur);
        if (v1->id_ == v2->id_ && cur < rep)
          v2->nodes_.insert(v2->nodes_.begin() + rep, val);
        else
          v2->nodes_.insert(v2->nodes_.begin() + rep + 1, val);
        current_cost_ += delta;
      }
      if (current_cost_ < best_cost_) {
        stag_ = stag_limit_;
        best_vehicles = vehicles_;
        best_cost_ = current_cost_;
      }
    }
  }
  vehicles_ = best_vehicles;
  cost = 0;
  for (const auto &v : vehicles_)
    cost += v.cost_;
  std::cout << "Cost: " << cost << '\n';
  for (const auto &i : nodes_) {
    if (!i.is_routed_) {
      std::cout << "Unreached node: " << '\n';
      std::cout << i << '\n';
    }
  }
  std::cout << "Solution valid: " << CheckSolutionValid() << '\n';
}
