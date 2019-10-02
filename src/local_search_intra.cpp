#include "local_search_intra.hpp"

void LocalSearchIntraSolution::Solve(){
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
  for(auto& v:vehicles) cost += v.cost;
  for(auto& v:vehicles){
    while(true){
      double delta = 0.0, cost_reduction, cost_increase;
      int cur, prev, next_c, rep, next_r, best_c, best_r;
      for(cur=1;cur<v.nodes.size()-1;cur++){
        prev = cur-1;
        next_c = cur+1;
        cost_reduction = distanceMatrix[v.nodes[prev]][v.nodes[next_c]]
                       - distanceMatrix[v.nodes[prev]][v.nodes[cur]]
                       - distanceMatrix[v.nodes[cur]][v.nodes[next_c]];
        for(rep=1;rep<v.nodes.size()-1;rep++){
          if(rep!=cur && rep!=cur-1){
            next_r = rep + 1;
            cost_increase = distanceMatrix[v.nodes[rep]][v.nodes[cur]]
                          + distanceMatrix[v.nodes[cur]][v.nodes[next_r]]
                          - distanceMatrix[v.nodes[rep]][v.nodes[next_r]];
            if(cost_increase + cost_reduction < delta){
                delta = cost_increase + cost_reduction;
                best_c = cur;
                best_r = rep;
            }
          }
        }
      }

      if(delta>-0.00001) break;
      else{
        int val_best_c = *(v.nodes.begin()+best_c);
        v.nodes.erase(v.nodes.begin()+best_c);
        if(best_c < best_r) v.nodes.insert(v.nodes.begin()+best_r, val_best_c);
        else v.nodes.insert(v.nodes.begin()+best_r+1, val_best_c);
        v.CalculateCost(distanceMatrix);
      }
    }
  }
  cost = 0;
  for(auto &v:vehicles) cost +=v.cost;
  std::cout << "Cost: " << cost << std::endl;
  for(auto& i:nodes){
    if(!i.is_routed){
      std::cout << "Unreached node: " << std::endl;
      i.PrintStatus();
    }
  }
}
