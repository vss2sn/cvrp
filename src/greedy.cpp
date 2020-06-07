/**
* @file greedy.cpp
* @author vss2sn
* @brief Contains the GreedySolution class
*/

#include "greedy.hpp"

GreedySolution::GreedySolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){};

GreedySolution::GreedySolution(Problem p)
  :Solution(p.nodes_, p.vehicles_, p.distanceMatrix_){};

void GreedySolution::Solve(){
  solution_string_ = std::to_string(depot_.id_);
  for(auto& v:vehicles_){
    while(true){
      Node closest_node = find_closest(v);
      if(closest_node.id_!=-1 && v.load_ - closest_node.demand_ >=0){
        v.load_ -= closest_node.demand_;
        v.cost_ += distanceMatrix_[v.nodes_.back()][closest_node.id_];
        v.nodes_.push_back(closest_node.id_);
        nodes_[closest_node.id_].is_routed_ = true;
        solution_string_ += ',' + std::to_string(closest_node.id_);
      }
      else{
        v.cost_ += distanceMatrix_[v.nodes_.back()][depot_.id_];
        v.nodes_.push_back(depot_.id_);
        solution_string_ += ',' + std::to_string(depot_.id_);
        break;
      }
    }
  }
#ifdef VISUALIZE
  mpp->publishToRViz(solution_string_, nodes_);
#endif // VISUALIZE

  double cost = 0;
  for(auto &v:vehicles_) cost +=v.cost_;
  std::cout << "Cost: " << cost << std::endl;

  for(auto& i:nodes_){
    if(!i.is_routed_){
      std::cout << "Unreached node: " << std::endl;
      i.PrintStatus();
    }
  }
  std::cout << "Solution valid: " << CheckSolutionValid()<< std::endl;
}
