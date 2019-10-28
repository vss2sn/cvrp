/**
* @file greedy.cpp
* @author vss2sn
* @brief Contains the GreedySolution class
*/

#include "greedy.hpp"

GreedySolution::GreedySolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){};

GreedySolution::GreedySolution(Problem p)
  :Solution(p.nodes, p.vehicles, p.distanceMatrix){};

void GreedySolution::Solve(){
  for(auto& v:vehicles){
    while(true){
      Node closest_node = find_closest(v, distanceMatrix, nodes);
      if(closest_node.id!=-1 && v.load - closest_node.demand >=0){
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
  double cost = 0;
  for(auto &v:vehicles) cost +=v.cost;
  std::cout << "Cost: " << cost << std::endl;

  for(auto& i:nodes){
    if(!i.is_routed){
      std::cout << "Unreached node: " << std::endl;
      i.PrintStatus();
    }
  }
  std::cout << "Solution valid: " << CheckSolutionValid()<< std::endl;
}
