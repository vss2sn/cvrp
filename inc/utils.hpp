/**
* @file utils.hpp
* @author vss2sn
* @brief Contains the structs, classes and functions used for the set up of the problem aand solution as well as some functions that aid in debugging.
*/

#ifndef UTILS_HPP
#define UTILS_HPP

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
#include <map>
#include <queue>
#include <unistd.h>

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
  int capacity;

  Problem(int noc = 1000, int demand_range = 40, int nov = 50, int capacity = 800, int grid_range = 1000, std::string distribution = "uniform", int n_clusters = 5, int cluster_range = 10);
};

class Solution{
public:
  std::vector<Node> nodes;
  std::vector<Vehicle> vehicles;
  std::vector<std::vector<double>> distanceMatrix;
  Node depot;
  int capacity;

  Solution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix);
  Solution(Problem p);
  void CreateInitialSolution();
  bool CheckSolutionValid();
  virtual void Solve(){};
  Node find_closest(Vehicle& v, std::vector<std::vector<double>>& distanceMatrix, std::vector<Node>& nodes);
};

struct tabu{
 int x;
 int pos_x;
 int y;
 int pos_y;
};

#endif
