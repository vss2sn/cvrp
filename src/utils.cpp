/**
 * @file utils.cpp
 * @author vss2sn
 * @brief Contains the structs, classes and functions used for the set up of the
 * problem aand solution as well as some functions that aid in debugging.
 */

#include <iostream>
#include <limits>
#include <random>

#include "cvrp/utils.hpp"

std::ostream &operator<<(std::ostream &os, const Node &node) {
  os << "Node Status" << '\n';
  os << "ID    : " << node.id_ << '\n';
  os << "X     : " << node.x_ << '\n';
  os << "Y     : " << node.y_ << '\n';
  os << "Demand: " << node.demand_ << '\n';
  os << '\n';
  return os;
}

void Vehicle::CalculateCost(const std::vector<std::vector<double>>& distanceMatrix) {
  cost_ = 0;
  for (size_t i = 0; i < nodes_.size() - 1; i++)
    cost_ += distanceMatrix[nodes_[i]][nodes_[i + 1]];
}

std::ostream &operator<<(std::ostream &os, const Vehicle &v) {
  os << "Vehicle Status" << '\n';
  os << "Cost    : " << v.cost_ << '\n';
  os << "ID      : " << v.id_ << '\n';
  os << "Load    : " << v.load_ << '\n';
  os << "Capacity: " << v.capacity_ << '\n';
  os << "Path    : ";
  // the nodes_.size()-1 limit is only added to ensure that there isnt a --->
  // after the last node, which is always the depot, ie node 0.
  for (size_t i = 0; i < v.nodes_.size() - 1; ++i) {
    os << v.nodes_[i] << " ---> ";
  }
  os << "0";
  os << '\n' << '\n';
  return os;
}

void PrintVehicleRoute(const Vehicle &v) {
  for (size_t i = 0; i < v.nodes_.size() - 1; ++i) {
    std::cout << v.nodes_[i] << " ---> ";
  }
  std::cout << "0";
  std::cout << '\n' << '\n';
}

Solution::Solution(const std::vector<Node>& nodes, const std::vector<Vehicle>& vehicles,
                   const std::vector<std::vector<double>>& distanceMatrix)
    : nodes_(nodes), vehicles_(vehicles), distanceMatrix_(distanceMatrix) {
  depot_ = nodes_[0];
  capacity_ = vehicles[0].load_;
}

Solution::Solution(const Problem& p) {
  nodes_ = p.nodes_;
  vehicles_ = p.vehicles_;
  distanceMatrix_ = p.distanceMatrix_;
  depot_ = nodes_[0];
  capacity_ = p.capacity_;
}

void Solution::CreateInitialSolution() {
  for (auto &v : vehicles_) {
    while (true) {
      Node closest_node = find_closest(v);
      if (closest_node.id_ != -1 &&
          v.load_ - closest_node.demand_ >= 0) { // }.2*capacity){
        v.load_ -= closest_node.demand_;
        v.cost_ += distanceMatrix_[v.nodes_.back()][closest_node.id_];
        v.nodes_.push_back(closest_node.id_);
        nodes_[closest_node.id_].is_routed_ = true;
      } else {
        v.cost_ += distanceMatrix_[v.nodes_.back()][depot_.id_];
        v.nodes_.push_back(depot_.id_);
        break;
      }
    }
  }
}

Node Solution::find_closest(const Vehicle &v) const {
  double cost = std::numeric_limits<double>::max();
  int id = -1;
  for (size_t j = 0; j < distanceMatrix_[0].size(); j++) {
    if (!nodes_[j].is_routed_ && nodes_[j].demand_ <= v.load_ &&
        distanceMatrix_[v.nodes_.back()][j] < cost) {
      cost = distanceMatrix_[v.nodes_.back()][j];
      id = j;
    }
  }
  if (id != -1)
    return nodes_[id];
  else
    return Node(0, 0, -1, 0);
}

bool Solution::CheckSolutionValid() const {
  // double cost = 0;
  std::vector<bool> check_nodes(nodes_.size(), false);
  check_nodes[0] = true;
  for (const auto &v : vehicles_) {
    int load = capacity_;
    for (const auto &n : v.nodes_) {
      load -= nodes_[n].demand_;
      check_nodes[n] = true;
    }
    if (load < 0)
      return false;
  }
  for (auto b : check_nodes) {
    if (!b)
      return false;
  }
  return true;
}

Problem::Problem(const int noc, const int demand_range, const int nov, const int capacity,
                 const int grid_range, std::string distribution, const int n_clusters,
                 const int cluster_range) {

  std::random_device rd;  // obtain a random number from hardware
  std::mt19937 eng(rd()); // seed the generator
  std::uniform_int_distribution<int> ran(-grid_range,
                                         grid_range);        // define the range
  std::uniform_int_distribution<int> ran_d(0, demand_range); // define the range
  std::uniform_int_distribution<int> ran_c(-cluster_range, cluster_range);
  Node depot(0, 0, 0, 0, true);
  this->capacity_ = capacity;

  nodes_.push_back(depot);

  if (distribution != "uniform" && distribution != "cluster")
    distribution = "uniform";
  if (distribution == "uniform")
    for (int i = 1; i <= noc; ++i)
      nodes_.emplace_back(ran(eng), ran(eng), i, ran_d(eng), false);
  else if (distribution == "cluster") {
    int id = 1;
    int n_p_c = noc / n_clusters;
    int remain = noc % n_clusters;
    for (int i = 0; i < n_clusters; i++) {
      int x = ran(eng);
      int y = ran(eng);
      for (int j = 0; j < n_p_c; j++) {
        nodes_.emplace_back(x + ran_c(eng), y + ran_c(eng), id, ran_d(eng), false);
        id++;
      }
    }
    int x = ran(eng);
    int y = ran(eng);
    for (int j = 0; j < remain; j++) {
      nodes_.emplace_back(x + ran_c(eng), y + ran_c(eng), id, ran_d(eng), false);
      id++;
    }
  }

  // for(const auto& n:nodes) std::cout << n << '\n';
  std::vector<double> tmp(nodes_.size());
  for (size_t i = 0; i < nodes_.size(); ++i)
    distanceMatrix_.push_back(tmp);
  for (size_t i = 0; i < nodes_.size(); ++i) {
    for (size_t j = i; j < nodes_.size(); ++j) {
      distanceMatrix_[i][j] =
          sqrt(double(pow((nodes_[i].x_ - nodes_[j].x_), 2) +
                      pow((nodes_[i].y_ - nodes_[j].y_), 2)));
      distanceMatrix_[j][i] = distanceMatrix_[i][j];
    }
  }

  int load = capacity_;
  for (int i = 0; i < nov; ++i) {
    vehicles_.emplace_back(i, load, capacity_);
    vehicles_[i].nodes_.push_back(0);
  }
}

void Solution::PrintSolution(const std::string &option) const {
  double total_cost = 0;
  for (const auto &v : vehicles_) {
    total_cost += v.cost_;
    if (option == "status") {
      PrintVehicleRoute(v);
    } else if (option == "route") {
      std::cout << "Vehicle ID: " << v.id_ << " | ";
      PrintVehicleRoute(v);
    }
  }
  bool valid = CheckSolutionValid();
  std::cout << "Total solution cost: " << total_cost << '\n';
  std::cout << "Solution validity  : " << valid << '\n';
  if (!valid) {
    for (const auto &i : nodes_) {
      if (!i.is_routed_) {
        std::cout << "Unreached node: " << '\n';
        std::cout << i << '\n';
      }
    }
  }
}
