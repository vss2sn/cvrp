/**
 * @file tabu_search.cpp
 * @author vss2sn
 * @brief Contains the TabuSearchSolution class
 */

#include "tabu_search.hpp"

TabuSearchSolution::TabuSearchSolution(
    const std::vector<Node>& nodes, const std::vector<Vehicle>& vehicles,
    const std::vector<std::vector<double>>& distanceMatrix, const int n_tabu)
    : Solution(nodes, vehicles, distanceMatrix) {
  this->n_tabu_ = n_tabu;
  CreateInitialSolution();
}

TabuSearchSolution::TabuSearchSolution(const Problem& p, const int n_tabu)
    : Solution(p.nodes_, p.vehicles_, p.distanceMatrix_) {
  this->n_tabu_ = n_tabu;
  CreateInitialSolution();
}

TabuSearchSolution::TabuSearchSolution(const Solution& s, const int n_tabu) : Solution(s) {
  this->n_tabu_ = n_tabu;
  if (!s.CheckSolutionValid()) {
    std::cout << "The input solution is invalid. Exiting." << '\n';
    exit(0);
  }
}

inline bool TabuSearchSolution::IsTabu(const int begin, const int end) {
  for (int i = begin; i <= end; i++) {
    if (tabu_list_set_.find(to_check_[i]) != tabu_list_set_.end())
      return true;
  }
  return false;
}

inline bool TabuSearchSolution::Aspiration(const double cost_increase,
                                           const double cost_reduction) {
  return new_cost_ + cost_increase + cost_reduction < best_cost_;
}

void TabuSearchSolution::Solve() {
  double cost = 0;
  for (const auto &v : vehicles_) {
    cost += v.cost_;
  }

  tabu_list_set_.clear();
  for (int i = 0; i < n_tabu_; i++) {
    tabu_list_queue_.emplace();
  }

  auto best_vehicles = vehicles_;
  int best_c = -1, best_r = -1;
  best_cost_ = cost;
  new_cost_ = cost;
  Vehicle *v_temp = nullptr;
  Vehicle *v_temp_2 = nullptr;

  for (auto& row : to_check_) {
    std::fill(std::begin(row), std::end(row), 0);
  }
  for (int c_it = 0; c_it < max_it; c_it++) {
    double delta = std::numeric_limits<double>::max();
    for (auto &v : vehicles_) {
      for (size_t cur = 1; cur < v.nodes_.size() - 1; cur++) {
        const int v_cur = v.nodes_[cur];
        const int v_prev = v.nodes_[cur - 1];
        const int v_next_c = v.nodes_[cur + 1];
        const double cost_reduction = distanceMatrix_[v_prev][v_next_c] - distanceMatrix_[v_prev][v_cur] - distanceMatrix_[v_cur][v_next_c];

        to_check_[0][0] = v_prev;
        to_check_[0][1] = v_cur;
        to_check_[1][0] = v_cur;
        to_check_[1][1] = v_prev;
        to_check_[2][0] = v_cur;
        to_check_[2][1] = v_next_c;
        to_check_[3][0] = v_next_c;
        to_check_[3][1] = v_cur;
        to_check_[4][1] = v_cur;
        to_check_[5][0] = v_cur;

        for (auto &v2 : vehicles_) {
          for (size_t rep = 0; rep < v2.nodes_.size() - 1; rep++) {
            const int v_rep = v2.nodes_[rep];
            const int v_next_r = v2.nodes_[rep + 1];
            if (v_rep != v_cur && (v.id_ != v2.id_ || v_rep != v_prev)) {
              to_check_[4][0] = v_rep;
              to_check_[5][1] = v_next_r;

              const double cost_increase = distanceMatrix_[v_rep][v_cur] +
                              distanceMatrix_[v_cur][v_next_r] -
                              distanceMatrix_[v_rep][v_next_r];
              if (cost_increase + cost_reduction < delta &&
                  (v2.load_ - nodes_[v_cur].demand_ >= 0 || v.id_ == v2.id_) &&
                  (!IsTabu(0, 5) || Aspiration(cost_increase, cost_reduction))
                 ) {
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
    if (delta == std::numeric_limits<double>::max()) {
      std::cout << "On iteration " << c_it << "No possible moves. Consider adjusting tabu list size.\n";
      break;
    }
    const int val_best_c = *std::next(v_temp->nodes_.begin(), best_c);
    v_temp->nodes_.erase(std::next(v_temp->nodes_.begin(), best_c));
    v_temp->CalculateCost(distanceMatrix_);
    if (v_temp->id_ == v_temp_2->id_ && best_c < best_r) {
      v_temp_2->nodes_.insert(std::next(v_temp_2->nodes_.begin(), best_r), val_best_c);
      tabu_list_set_.insert({v_temp_2->nodes_[best_r - 1], val_best_c});
      tabu_list_queue_.push({v_temp_2->nodes_[best_r - 1], val_best_c});
    } else {
      v_temp_2->nodes_.insert(std::next(v_temp_2->nodes_.begin(), best_r + 1), val_best_c);
      tabu_list_set_.insert({v_temp_2->nodes_[best_r], val_best_c});
      tabu_list_queue_.push({v_temp_2->nodes_[best_r], val_best_c});
    }
    tabu_list_set_.insert({v_temp->nodes_[best_c - 1], val_best_c});
    tabu_list_queue_.push({v_temp->nodes_[best_c - 1], val_best_c});

    v_temp_2->CalculateCost(distanceMatrix_);
    v_temp->load_ += nodes_[val_best_c].demand_;
    v_temp_2->load_ -= nodes_[val_best_c].demand_;

    new_cost_ = 0;
    for (const auto &v : vehicles_) {
      new_cost_ += v.cost_;
    }

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
