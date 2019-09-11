#include "greedy.hpp"

void GreedySolution::Solve(){
  // std::cout << "Greedy Solution" << std::endl;
  // std::cout << "---------------" << std::endl;
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
    // v.PrintStatus();

    // v.PrintRoute();
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
  // std::cout << "---------------------------" << std::endl;
  // std::cout << "End of greedy solution" << std::endl;
}

// int main(){
//
//   Problem p;
//   GreedySolution g(p);
//   g.Solve();
//
//   return 0;
// }
