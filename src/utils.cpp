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
  for(int i = 0; i < nodes.size()-1; ++i) std::cout << nodes[i] << " ---> ";
  std::cout << "0";
  std::cout << std::endl << std::endl;
}

void Route::PrintRoute(){
  std::cout << "Path    : ";
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
  for(int i = 0; i < nodes.size()-1; ++i) std::cout << nodes[i] << " ---> ";
  std::cout << "0";
  std::cout << std::endl << std::endl;
}

Solution::Solution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :nodes(nodes), vehicles(vehicles), distanceMatrix(distanceMatrix){
  depot = nodes[0];
}

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
