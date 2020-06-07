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
#include <cstdio>
#include <ctime>
#include <cxxabi.h>
#include <typeinfo>


#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <cmath>

#ifdef VISUALIZE

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "visualization_msgs/msg/marker.hpp"

#endif //VISUALIZE

/**
* @brief Class node
* @details Contains the x, y coordinates of the locationof the node, its id, its demand, and whether it has been added to the routes of any of the vehicles
*/
class Node{
public:
  int x_, y_, id_, demand_;
  bool is_routed_;

  /**
  * @brief Consructor
  * @param x x coordinate
  * @param y y coordinate
  * @param id node id
  * @param demand node demand
  * @param is_routed has the node been included in a route
  * @return no return parameter
  * @details Constructor for a node
  */
  Node(int x = 0,
       int y = 0,
       int id = 0,
       int demand = 0,
       bool is_routed = true)
       :x_(x), y_(y), id_(id), demand_(demand), is_routed_(is_routed){}

  /**
  * @brief print status of node
  * @details prints the status of the node, ie, each of its elements
  */
  void PrintStatus();
};

class Route{
public:
  double cost_ = 0;
  std::vector<int> nodes_;

  /**
  * @brief print status of route
  * @details prints the status of the route, ie, each of its elements
  */
  void PrintStatus();

  /**
  * @brief prints the route
  * @details prints the route, ie, each of its elements in order
  */
  void PrintRoute();

  /**
  * @brief Calculates cost of the route
  * @param distanceMatrix Holds the distances between each pair of nodes
  * @return void
  * @details Calculates cost of the route
  */
  void CalculateCost(std::vector<std::vector<double>> distanceMatrix);
};


class Vehicle: public Route{
public:
  int id_, load_, capacity_;

  /**
  * @brief Constructor
  * @param id Vehicle id
  * @param load Current vehicle load
  * @param capacity Maximum vehicle capacity (initial load)
  * @return no return value
  * @details Constructor of vehicle class
  */
  Vehicle(int id = 0,
          int load = 0,
          int capacity = 0)
          : id_(id), load_(load), capacity_(capacity){}

  /**
  * @brief print status of vehicle
  * @details prints the status of the vehicle, ie, each of its elements
  */
  void PrintStatus();
};

class Problem{
public:
  std::vector<Node> nodes_;
  std::vector<Vehicle> vehicles_;
  std::vector<std::vector<double>> distanceMatrix_;
  Node depot_;
  int capacity_;

  /**
  * @brief Constructor
  * @param noc number of nodes (centres/dropoff points)
  * @param demand_range max demand of each node (random value between 0 and this value)
  * @param nov number of vehicles
  * @param capacity maximum capacityof each vehicle
  * @param grid_range maximum x and y coordinates of any node
  * @param distribution distribution of nodes. Can be either clustered or uniform.
  * @param n_clusters If clustered, number of clusters the nodes are divided into
  * @param cluster_range If clustered, maximum distance from center of cluster.
  * @return no return value
  * @details Constructor for problem class
  */
  Problem(int noc = 1000,
          int demand_range = 40,
          int nov = 50,
          int capacity = 800,
          int grid_range = 1000,
          std::string distribution = "uniform",
          int n_clusters = 5,
          int cluster_range = 10);
};

// Solution class should not call problems's constructor so not inheriting.
class Solution{
public:
  std::vector<Node> nodes_;
  std::vector<Vehicle> vehicles_;
  std::vector<std::vector<double>> distanceMatrix_;
  Node depot_;
  int capacity_;
  std::string solution_string_ = "";

  /**
  * @brief Constructor
  * @param nodes Vector of all nodes
  * @param vehicles Vector of vehicles
  * @param distanceMatrix Matrix containing distance between each pair of nodes
  * @return no return type
  * @details Constructor for solution class
  */
  Solution(std::vector<Node> nodes,
           std::vector<Vehicle> vehicles,
           std::vector<std::vector<double>> distanceMatrix);

  /**
  * @brief Constructor
  * @param p Instance of Problem class defining the problem parameters
  * @return no return type
  * @details Constructor for solution class
  */
  Solution(Problem p);

  /**
  * @brief Creates initial solution
  * @return void
  * @details Creates initial solution, currently set to greedy.
  */
  void CreateInitialSolution();

  /**
  * @brief Check whether the solution is valid
  * @return bool True is solution is valid
  * @details Check whether the solution is valid by checking whether all the nodes are reached and whether the total edmand of all the nodes on any of the routes exceed the capacity of the respective vehicles
  */
  bool CheckSolutionValid();

  /**
  * @brief Virtual function overloaded by solution classes to solve the given problem.
  * @return void
  * @details Virtual function overloaded by solution classes to solve the given problem.
  */
  virtual void Solve();

  /**
  * @brief find closest node
  * @param v vehicle under consideration
  * @return Node nearest node
  * @details Finds the node nearest to the last node in the route of the vehicle under constideration that has not been routed already
  */
  Node find_closest(Vehicle& v);

  /**
  * @brief Prints the status of solution
  * @param option allows the option to print routes or vehicle statuses
  * @return void
  * @details Prints the solution status including cost, the vehicle status, and solution validity
  */
  void PrintSolution(const std::string& option="");

};

#ifdef VISUALIZE

/**
* @brief Class publisher node
* @details Allows the visualization of the solution
*/
class MinimalPublisher : public rclcpp::Node
{
public:
  /**
  * @brief Constructor
  */
  MinimalPublisher();

  /**
  * @brief publishes solution to rviz
  * @param solution_string containing node ids in order of nodes visited
  * @param nodes of nodes to get x,y coordinates of nodes
  * @return void
  */
  void publishToRViz(const std::string& solution_string, const std::vector<::Node>& nodes);

  /**
  * @brief publisher for publishing the solution passed in
  */
  rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr publisher_;
};

extern std::shared_ptr<MinimalPublisher> mpp;

#endif // VISUALIZE

#endif // UTILS_HPP
