/**
 * @file simulated_annealing.cpp
 * @author vss2sn
 * @brief Contains the SimulatedAnnealingSolution class
 */

#include "cvrp/simulated_annealing.hpp"

#include <cmath>
#include <iostream>
#include <numeric>

constexpr double margin_of_error = 0.0000000001;

SimulatedAnnealingSolution::SimulatedAnnealingSolution(
    const std::vector<Node>& nodes, const std::vector<Vehicle>& vehicles,
    const std::vector<std::vector<double>>& distanceMatrix,
    const int stag_limit, const double init_temp, const double cooling_rate,
    const int n_reheats)
    : Solution(nodes, vehicles, distanceMatrix),
      stag_limit_(stag_limit),
      init_temp_(init_temp),
      cooling_rate_(cooling_rate),
      n_reheats_(n_reheats) {
  CreateInitialSolution();
}

SimulatedAnnealingSolution::SimulatedAnnealingSolution(
    const Problem& p, const int stag_limit, const double init_temp,
    const double cooling_rate, const int n_reheats)
    : Solution(p.nodes_, p.vehicles_, p.distanceMatrix_),
      stag_limit_(stag_limit),
      init_temp_(init_temp),
      cooling_rate_(cooling_rate),
      n_reheats_(n_reheats) {
  CreateInitialSolution();
}

SimulatedAnnealingSolution::SimulatedAnnealingSolution(
    const Solution& s, const int stag_limit, const double init_temp,
    const double cooling_rate, const int n_reheats)
    : Solution(s),
      stag_limit_(stag_limit),
      init_temp_(init_temp),
      cooling_rate_(cooling_rate),
      n_reheats_(n_reheats) {
  if (!s.CheckSolutionValid()) {
    std::cout << "The input solution is invalid. Exiting." << '\n';
    exit(0);
  }
}

inline bool SimulatedAnnealingSolution::AllowMove(const double delta,
                                                  const double temp) {
  return (delta < -margin_of_error) ||
         ((static_cast<double>(rand()) / RAND_MAX) < std::exp(-delta / temp));
}

void SimulatedAnnealingSolution::Solve() {
  double cost = std::accumulate(
      std::begin(vehicles_), std::end(vehicles_), 0.0,
      [](const double sum, const Vehicle& v) { return sum + v.cost_; });
  auto best_vehicles = vehicles_;
  double best_cost = cost;
  double current_cost = cost;
  for (int r = 0; r < n_reheats_; r++) {
    // std::cout << "Reheat number: " << r << '\n';
    int stag = stag_limit_;
    double temp = init_temp_;
    while (--stag >= 0) {
      temp *= cooling_rate_;
      const int n_vehicles = vehicles_.size();
      Vehicle& v1 = vehicles_[rand() % n_vehicles];
      Vehicle& v2 = vehicles_[rand() % n_vehicles];
      size_t cur = 0;
      if (v1.nodes_.size() > 2) {
        // do not select trailing zero or starting zero
        cur = rand() % (v1.nodes_.size() - 2) + 1;
      } else {
        continue;
      }
      const size_t rep =
          rand() % (v2.nodes_.size() - 1);  // do not select trailing zero
      if (v1.id_ == v2.id_ && (cur == rep + 1 || cur == rep)) {
        continue;
      }
      const size_t prev = cur - 1;
      const size_t next_c = cur + 1;
      const size_t next_r = rep + 1;
      const double cost_reduction =
          distanceMatrix_[v1.nodes_[prev]][v1.nodes_[next_c]] -
          distanceMatrix_[v1.nodes_[prev]][v1.nodes_[cur]] -
          distanceMatrix_[v1.nodes_[cur]][v1.nodes_[next_c]];
      const double cost_increase =
          distanceMatrix_[v2.nodes_[rep]][v1.nodes_[cur]] +
          distanceMatrix_[v1.nodes_[cur]][v2.nodes_[next_r]] -
          distanceMatrix_[v2.nodes_[rep]][v2.nodes_[next_r]];
      const double delta = cost_increase + cost_reduction;
      if ((v2.load_ - nodes_[v1.nodes_[cur]].demand_ >= 0 ||
           v1.id_ == v2.id_) &&
          AllowMove(delta, temp)) {
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
      if (current_cost < best_cost) {
        stag = stag_limit_;
        best_vehicles = vehicles_;
        best_cost = current_cost;
      }
    }
  }
  vehicles_ = best_vehicles;
  cost = std::accumulate(
      std::begin(vehicles_), std::end(vehicles_), 0.0,
      [](const double sum, const Vehicle& v) { return sum + v.cost_; });
  std::cout << "Cost: " << cost << '\n';
  for (const auto& i : nodes_) {
    if (!i.is_routed_) {
      std::cout << "Unreached node: " << '\n';
      std::cout << i << '\n';
    }
  }
  std::cout << "Solution valid: " << CheckSolutionValid() << '\n';
}
