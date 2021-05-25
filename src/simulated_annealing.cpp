/**
 * @file simulated_annealing.cpp
 * @author vss2sn
 * @brief Contains the SimulatedAnnealingSolution class
 */

#include <cmath>
#include <iostream>

#include "cvrp/simulated_annealing.hpp"

SimulatedAnnealingSolution::SimulatedAnnealingSolution(
    const std::vector<Node>& nodes, const std::vector<Vehicle>& vehicles,
    const std::vector<std::vector<double>>& distanceMatrix, const int stag_limit,
    const double init_temp, const double cooling_rate)
    : Solution(nodes, vehicles, distanceMatrix),
      stag_limit_ (stag_limit), max_temp_ (init_temp),
      cooling_rate_ (cooling_rate) {
  CreateInitialSolution();
}

SimulatedAnnealingSolution::SimulatedAnnealingSolution(const Problem& p,
                                                       const int stag_limit,
                                                       const double init_temp,
                                                       const double cooling_rate)
    : Solution(p.nodes_, p.vehicles_, p.distanceMatrix_),
    stag_limit_ (stag_limit), max_temp_ (init_temp),
    cooling_rate_ (cooling_rate) {
  CreateInitialSolution();
}

SimulatedAnnealingSolution::SimulatedAnnealingSolution(const Solution& s,
                                                       const int stag_limit,
                                                       const double init_temp,
                                                       const double cooling_rate)
    : Solution(s),
    stag_limit_ (stag_limit), max_temp_ (init_temp),
    cooling_rate_ (cooling_rate) {
  if (!s.CheckSolutionValid()) {
    std::cout << "The input solution is invalid. Exiting." << '\n';
    exit(0);
  }
}

inline bool SimulatedAnnealingSolution::AllowMove(const double delta, const double temp) {
  if (delta < -0.0000000001)
    return true;
  else if (((double)rand() / RAND_MAX) < std::exp(-delta / temp))
    return true;
  else
    return false;
}

void SimulatedAnnealingSolution::Solve() {
  double cost = 0;
  double temp = 0;
  for (const auto &v : vehicles_) {
    cost += v.cost_;
  }
  auto best_vehicles = vehicles_;
  best_cost_ = cost;
  double current_cost = cost;
  // Vehicle *v1, *v2;
  for (int r = 0; r < n_reheats_; r++) {
    // std::cout << "Reheat number: " << r << '\n';
    int stag = stag_limit_;
    temp = max_temp_;
    while (--stag >= 0) {
      temp *= cooling_rate_;
      const int n_vehicles = vehicles_.size();
      Vehicle& v1 = vehicles_[rand() % n_vehicles];
      Vehicle& v2 = vehicles_[rand() % n_vehicles];
      int cur = 0;
      if (v1.nodes_.size() > 2) {
        // do not select trailing zero or starting zero
        cur = rand() % (v1.nodes_.size() - 2) +1;
      } else {
        continue;
      }
      const int rep = rand() % (v2.nodes_.size() - 1); // do not select trailing zero
      if (v1.id_ == v2.id_ && (cur == rep + 1 || cur == rep)) {
        continue;
      }
      const int prev = cur - 1;
      const int next_c = cur + 1;
      const int next_r = rep + 1;
      const double cost_reduction = distanceMatrix_[v1.nodes_[prev]][v1.nodes_[next_c]] -
                       distanceMatrix_[v1.nodes_[prev]][v1.nodes_[cur]] -
                       distanceMatrix_[v1.nodes_[cur]][v1.nodes_[next_c]];
      const double cost_increase = distanceMatrix_[v2.nodes_[rep]][v1.nodes_[cur]] +
                      distanceMatrix_[v1.nodes_[cur]][v2.nodes_[next_r]] -
                      distanceMatrix_[v2.nodes_[rep]][v2.nodes_[next_r]];
      const double delta = cost_increase + cost_reduction;
      if ((v2.load_ - nodes_[v1.nodes_[cur]].demand_ >= 0 ||
           v1.id_ == v2.id_) && AllowMove(delta, temp)) {
        v1.load_ += nodes_[v1.nodes_[cur]].demand_;
        v2.load_ -= nodes_[v1.nodes_[cur]].demand_;
        v1.cost_ += cost_reduction;
        v2.cost_ += cost_increase;
        const int val = v1.nodes_[cur];
        v1.nodes_.erase(v1.nodes_.begin() + cur);
        if (v1.id_ == v2.id_ && cur < rep) {
          v2.nodes_.insert(v2.nodes_.begin() + rep, val);
        } else {
          v2.nodes_.insert(v2.nodes_.begin() + rep + 1, val);
        }
        current_cost += delta;
      }
      if (current_cost < best_cost_) {
        stag = stag_limit_;
        best_vehicles = vehicles_;
        best_cost_ = current_cost;
      }
    }
  }
  vehicles_ = best_vehicles;
  cost = 0;
  for (const auto &v : vehicles_) {
    cost += v.cost_;
  }
  std::cout << "Cost: " << cost << '\n';
  for (const auto &i : nodes_) {
    if (!i.is_routed_) {
      std::cout << "Unreached node: " << '\n';
      std::cout << i << '\n';
    }
  }
  std::cout << "Solution valid: " << CheckSolutionValid() << '\n';
}
