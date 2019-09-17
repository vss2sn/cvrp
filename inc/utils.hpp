#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#include <climits>
#include <unordered_set>
#include <set>
#include <memory>
#include <algorithm>
#include <iomanip>

class Node{
public:
  int x, y, id, demand;
  bool is_routed;

  Node(int x = 0, int y = 0, int id = 0, int demand = 0, bool is_routed = true):x(x), y(y), id(id), demand(demand), is_routed(is_routed){}

  void PrintStatus();
};

class Route{
public:
  double cost = 0;
  std::vector<int> nodes;

  void PrintStatus();
  void PrintRoute();
  void CalculateCost(std::vector<std::vector<double>> distanceMatrix);
};


class Vehicle: public Route{
public:
  int id, load, capacity;

  Vehicle(int id = 0, int load = 0, int capacity = 0) : id(id), load(load), capacity(capacity){}

  void PrintStatus();

};

class Problem{
public:
  std::vector<Node> nodes;
  std::vector<Vehicle> vehicles;
  std::vector<std::vector<double>> distanceMatrix;
  Node depot;

  Problem(int noc = 50, int demand_range = 40, int nov = 25, int capacity = 80, int grid_range = 1000, std::string distribution = "uniform", int n_clusters = 5, int cluster_range = 10);
};

class Solution{
public:
  std::vector<Node> nodes;
  std::vector<Vehicle> vehicles;
  std::vector<std::vector<double>> distanceMatrix;
  Node depot;

  Solution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix);
  Solution(Problem p);
  virtual void Solve(){};
  Node find_closest(Vehicle& v, std::vector<std::vector<double>>& distanceMatrix, std::vector<Node>& nodes);
};
#endif
