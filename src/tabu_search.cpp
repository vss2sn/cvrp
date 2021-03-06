/**
 * @file tabu_search.cpp
 * @author vss2sn
 * @brief Contains the TabuSearchSolution class
 */

#include "cvrp/tabu_search.hpp"

#include <iostream>
#include <numeric>

TabuSearchSolution::TabuSearchSolution(
    const std::vector<Node> &nodes, const std::vector<Vehicle> &vehicles,
    const std::vector<std::vector<double>> &distanceMatrix, const int n_tabu,
    const int max_it)
    : Solution(nodes, vehicles, distanceMatrix),
      n_tabu_(n_tabu),
      max_it_(max_it) {
  CreateInitialSolution();
}

TabuSearchSolution::TabuSearchSolution(const Problem &p, const int n_tabu,
                                       const int max_it)
    : Solution(p.nodes_, p.vehicles_, p.distanceMatrix_),
      n_tabu_(n_tabu),
      max_it_(max_it) {
  CreateInitialSolution();
}

TabuSearchSolution::TabuSearchSolution(const Solution &s, const int n_tabu,
                                       const int max_it)
    : Solution(s), n_tabu_(n_tabu), max_it_(max_it) {
  if (!s.CheckSolutionValid()) {
    std::cout << "The input solution is invalid. Exiting." << '\n';
    exit(0);
  }
}

inline bool TabuSearchSolution::IsTabu(const std::pair<int, int> &p) const {
  return tabu_list_set_.find(p) != std::end(tabu_list_set_);
}

inline bool TabuSearchSolution::Aspiration(const double cost_increase,
                                           const double cost_reduction) const {
  return new_cost_ + cost_increase + cost_reduction < best_cost_;
}

void TabuSearchSolution::Solve() {
  double cost = std::accumulate(
      std::begin(vehicles_), std::end(vehicles_), 0.0,
      [](const double sum, const Vehicle &v) { return sum + v.cost_; });
  tabu_list_set_.clear();
  for (int i = 0; i < n_tabu_; i++) {
    tabu_list_queue_.emplace();
  }

  auto best_vehicles = vehicles_;
  best_cost_ = cost;
  new_cost_ = cost;
  Vehicle *v_temp = nullptr;
  Vehicle *v_temp_2 = nullptr;

  for (int c_it = 0; c_it < max_it_; c_it++) {
    int best_c = -1;
    int best_r = -1;
    double delta = std::numeric_limits<double>::max();
    for (auto &v : vehicles_) {
      for (size_t cur = 1; cur < v.nodes_.size() - 1; cur++) {
        const int v_cur = v.nodes_[cur];
        const int v_prev = v.nodes_[cur - 1];
        const int v_next_c = v.nodes_[cur + 1];
        const double cost_reduction = distanceMatrix_[v_prev][v_next_c] -
                                      distanceMatrix_[v_prev][v_cur] -
                                      distanceMatrix_[v_cur][v_next_c];
        const bool is_tabu_1 =
            IsTabu({v_prev, v_cur}) || IsTabu({v_cur, v_prev}) ||
            IsTabu({v_cur, v_next_c}) || IsTabu({v_next_c, v_cur});

        for (auto &v2 : vehicles_) {
          for (size_t rep = 0; rep < v2.nodes_.size() - 1; rep++) {
            const int v_rep = v2.nodes_[rep];
            const int v_next_r = v2.nodes_[rep + 1];
            if (v_rep != v_cur && (v.id_ != v2.id_ || v_rep != v_prev)) {
              const bool is_tabu_2 =
                  IsTabu({v_rep, v_cur}) || IsTabu({v_cur, v_next_r});
              const double cost_increase = distanceMatrix_[v_rep][v_cur] +
                                           distanceMatrix_[v_cur][v_next_r] -
                                           distanceMatrix_[v_rep][v_next_r];
              if ((cost_increase + cost_reduction < delta) &&
                  (v2.load_ - nodes_[v_cur].demand_ >= 0 || v.id_ == v2.id_) &&
                  (!(is_tabu_1 || is_tabu_2) ||
                   Aspiration(cost_increase, cost_reduction))) {
                delta = cost_increase + cost_reduction;
                best_c = cur;
                best_r = rep;
                v_temp_2 = &v2;
                v_temp = &v;
              }
            }
          }
        }
      }
    }
    if (delta == std::numeric_limits<double>::max() || v_temp == nullptr ||
        v_temp_2 == nullptr) {
      std::cout << "On iteration " << c_it
                << "No possible moves. Consider adjusting tabu list size.\n";
      break;
    }
    const int val_best_c = *std::next(v_temp->nodes_.begin(), best_c);
    v_temp->nodes_.erase(std::next(v_temp->nodes_.begin(), best_c));
    v_temp->CalculateCost(distanceMatrix_);
    if (v_temp->id_ == v_temp_2->id_ && best_c < best_r) {
      v_temp_2->nodes_.insert(std::next(v_temp_2->nodes_.begin(), best_r),
                              val_best_c);
      tabu_list_set_.insert({v_temp_2->nodes_[best_r - 1], val_best_c});
      tabu_list_queue_.push({v_temp_2->nodes_[best_r - 1], val_best_c});
    } else {
      v_temp_2->nodes_.insert(std::next(v_temp_2->nodes_.begin(), best_r + 1),
                              val_best_c);
      tabu_list_set_.insert({v_temp_2->nodes_[best_r], val_best_c});
      tabu_list_queue_.push({v_temp_2->nodes_[best_r], val_best_c});
    }
    tabu_list_set_.insert({v_temp->nodes_[best_c - 1], val_best_c});
    tabu_list_queue_.push({v_temp->nodes_[best_c - 1], val_best_c});

    v_temp_2->CalculateCost(distanceMatrix_);
    v_temp->load_ += nodes_[val_best_c].demand_;
    v_temp_2->load_ -= nodes_[val_best_c].demand_;

    new_cost_ = std::accumulate(
        std::begin(vehicles_), std::end(vehicles_), 0.0,
        [](const double sum, const Vehicle &v) { return sum + v.cost_; });

    if (new_cost_ < best_cost_) {
      best_vehicles = vehicles_;
      best_cost_ = new_cost_;
    }
    tabu_list_set_.erase(tabu_list_queue_.front());
    tabu_list_queue_.pop();
    tabu_list_set_.erase(tabu_list_queue_.front());
    tabu_list_queue_.pop();
  }
  vehicles_ = best_vehicles;
  cost = std::accumulate(
      std::begin(vehicles_), std::end(vehicles_), 0.0,
      [](const double sum, const Vehicle &v) { return sum + v.cost_; });
  std::cout << "Cost: " << cost << '\n';
  for (const auto &i : nodes_) {
    if (!i.is_routed_) {
      std::cout << "Unreached node: " << '\n';
      std::cout << i << '\n';
    }
  }
  std::cout << "Solution valid: " << CheckSolutionValid() << '\n';
}
