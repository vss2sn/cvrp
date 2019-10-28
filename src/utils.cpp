/**
* @file utils.cpp
* @author vss2sn
* @brief Contains the structs, classes and functions used for the set up of the problem aand solution as well as some functions that aid in debugging.
*/

#include "utils.hpp"

void Node::PrintStatus(){
  std::cout << "Node Status" << std::endl;
  std::cout << "ID    : " << id << std::endl;
  std::cout << "X     : " << x << std::endl;
  std::cout << "Y     : " << y << std::endl;
  std::cout << "Demand: " << demand << std::endl;
  std::cout << std::endl;
}

void Route::PrintStatus(){
  std::cout << "Route Status" << std::endl;
  std::cout << "Cost    : " << cost << std::endl;
  std::cout << "Path    : ";
  // the nodes.size()-1 limit is only added to ensure that there isnt a --->
  // after the last node, which is always the depot, ie node 0.
  for(int i = 0; i < nodes.size()-1; ++i) std::cout << nodes[i] << " ---> ";
  std::cout << "0";
  std::cout << std::endl << std::endl;
}

void Route::PrintRoute(){
  std::cout << "Path    : ";
  // the nodes.size()-1 limit is only added to ensure that there isnt a --->
  // after the last node, which is always the depot, ie node 0.
  for(int i = 0; i < nodes.size()-1; ++i) std::cout << nodes[i] << " ---> ";
  std::cout << "0";
  std::cout << std::endl << std::endl;
}

void Route::CalculateCost(std::vector<std::vector<double>> distanceMatrix){
  cost = 0;
  for(int i=0;i<nodes.size()-1;i++) cost+=distanceMatrix[nodes[i]][nodes[i+1]];
}

void Vehicle::PrintStatus(){
  std::cout << "Vehicle Status" << std::endl;
  std::cout << "Cost    : " << cost << std::endl;
  std::cout << "ID      : " << id << std::endl;
  std::cout << "Load    : " << load << std::endl;
  std::cout << "Capacity: " << capacity << std::endl;
  std::cout << "Path    : "   ;
  // the nodes.size()-1 limit is only added to ensure that there isnt a --->
  // after the last node, which is always the depot, ie node 0.
  for(int i = 0; i < nodes.size()-1; ++i) std::cout << nodes[i] << " ---> ";
  std::cout << "0";
  std::cout << std::endl << std::endl;
}

Solution::Solution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :nodes(nodes), vehicles(vehicles), distanceMatrix(distanceMatrix){
  depot = nodes[0];
  capacity = vehicles[0].load;
}

Solution::Solution(Problem p){
  nodes = p.nodes;
  vehicles = p.vehicles;
  distanceMatrix = p.distanceMatrix;
  depot = nodes[0];
  capacity = p.capacity;
}

void Solution::CreateInitialSolution(){
  for(auto& v:vehicles){
    while(true){
      Node closest_node = find_closest(v, distanceMatrix, nodes);
      if(closest_node.id!=-1 && v.load - closest_node.demand >=0){//}.2*capacity){
        v.load -= closest_node.demand;
        v.cost += distanceMatrix[v.nodes.back()][closest_node.id];
        v.nodes.push_back(closest_node.id);
        nodes[closest_node.id].is_routed = true;
      }
      else{
        v.cost += distanceMatrix[v.nodes.back()][depot.id];
        v.nodes.push_back(depot.id);
        break;
      }
    }
  }
}

void Solution::Solve(){}

Node Solution::find_closest(Vehicle& v, std::vector<std::vector<double>>& distanceMatrix, std::vector<Node>& nodes){
    double cost = INT_MAX;
    int id = -1;
    for(int j=0; j < distanceMatrix[0].size(); j++){
      if(!nodes[j].is_routed && nodes[j].demand <= v.load && distanceMatrix[v.nodes.back()][j] < cost){
        cost = distanceMatrix[v.nodes.back()][j];
        id = j;
      }
    }
    if(id!=-1) return nodes[id];
    else return Node(0,0,-1,0);
}

bool Solution::CheckSolutionValid(){
  // double cost = 0;
  std::vector<bool> check_nodes(nodes.size(), false);
  check_nodes[0]=true;
  for(auto&v:vehicles){
    int load = capacity;
    for(auto&n:v.nodes){
      load-=nodes[n].demand;
      check_nodes[n] = true;
    }
    if(load<0) return false;
  }
  for(auto b:check_nodes){
    if(!b) return false;
  }
  return true;
}

Problem::Problem(int noc, int demand_range, int nov, int capacity, int grid_range, std::string distribution, int n_clusters, int cluster_range){

  std::random_device rd; // obtain a random number from hardware
  std::mt19937 eng(rd()); // seed the generator
  std::uniform_int_distribution<int> ran(-grid_range,grid_range); // define the range
  std::uniform_int_distribution<int> ran_d(0,demand_range); // define the range
  std::uniform_int_distribution<int> ran_c(-cluster_range,cluster_range);
  Node depot(0, 0, 0, 0, true);
  this->capacity = capacity;

  nodes.push_back(depot);

  if(distribution != "uniform" && distribution != "cluster") distribution = "uniform";
  if(distribution == "uniform") for(int i = 1; i <=noc; ++i) nodes.emplace_back(ran(eng), ran(eng), i, ran_d(eng), false);
  else if(distribution == "cluster"){
    int id = 1;
    int n_p_c = noc/n_clusters;
    int remain = noc%n_clusters;
    for(int i=0;i<n_clusters;i++){
      int x = ran(eng);
      int y = ran(eng);
      for(int j=0;j<n_p_c;j++){
        nodes.emplace_back(x + ran_c(eng), y + ran_c(eng), id, ran_d(eng), false);
        // nodes.back().PrintStatus();
        id++;
      }
    }
    int x = ran(eng);
    int y = ran(eng);
    for(int j=0;j<remain;j++){
      nodes.emplace_back(x + ran_c(eng), y + ran_c(eng), id, ran_d(eng), false);
      id++;
    }
  }

  // for(auto& n:nodes) n.PrintStatus();
  std::vector<double> tmp(nodes.size());
  for(int i=0; i<nodes.size(); ++i) distanceMatrix.push_back(tmp);
  for(int i=0; i<nodes.size(); ++i){
    for(int j=i; j < nodes.size(); ++j){
      distanceMatrix[i][j] = sqrt(double(pow((nodes[i].x - nodes[j].x),2)
                                       + pow((nodes[i].y - nodes[j].y),2)));
      distanceMatrix[j][i] = distanceMatrix[i][j];
    }
  }

  int load = capacity;
  for(int i=0; i<nov; ++i){
    vehicles.emplace_back(i, load, capacity);
    vehicles[i].nodes.push_back(0);
  }
}
