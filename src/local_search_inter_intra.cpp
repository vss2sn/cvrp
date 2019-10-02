#include "local_search_inter_intra.hpp"

void LocalSearchInterIntraSolution::Solve(){

  capacity = vehicles[0].load;
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

  double cost = 0;
  for(auto& v:vehicles) cost += v.cost;
  // std::cout << "Costg: " << cost << std::endl;
  while(true){
    double delta = 0.0, cost_reduction, cost_increase, bcr, bci;
    int cur, prev, next_c, rep, next_r, best_c, best_r;
    Vehicle *v_temp_2, *v_temp;
    for(auto& v:vehicles){
      for(auto& v2:vehicles){
        for(cur=1;cur<v.nodes.size()-1;cur++){
          prev = cur-1;
          next_c = cur+1;
          cost_reduction = distanceMatrix[v.nodes[prev]][v.nodes[next_c]]
                         - distanceMatrix[v.nodes[prev]][v.nodes[cur]]
                         - distanceMatrix[v.nodes[cur]][v.nodes[next_c]];
          for(rep=0;rep<v2.nodes.size()-1;rep++){
            if(v2.nodes[rep]!=v.nodes[cur] && (v.id!=v2.id || v2.nodes[rep]!=v.nodes[cur-1])){
              next_r = rep + 1;
              cost_increase = distanceMatrix[v2.nodes[rep]][v.nodes[cur]]
                            + distanceMatrix[v.nodes[cur]][v2.nodes[next_r]]
                            - distanceMatrix[v2.nodes[rep]][v2.nodes[next_r]];
              if(cost_increase + cost_reduction < delta && (v2.load - nodes[v.nodes[cur]].demand >= 0
              || v.id == v2.id)){
                  bci = cost_increase;
                  bcr = cost_reduction;
                  delta = cost_increase + cost_reduction;
                  best_c = cur;
                  best_r = rep;
                  v_temp_2 = &v2;
                  v_temp = &v;
              }
            }
          }
        }
      }
    }
    // std::cout << "Cost increase:  " << bci << std::endl;
    // std::cout << "Cost reduction: " << bcr << std::endl;
    // std::cout << distanceMatrix[v_temp_2->nodes[best_r]][v_temp->nodes[best_c]]
    //           << " + " << distanceMatrix[v_temp->nodes[best_c]][v_temp_2->nodes[best_r + 1]]
    //           << " - " << distanceMatrix[v_temp_2->nodes[best_r]][v_temp_2->nodes[best_r + 1]]
    //               << std::endl;
    // std::cout << distanceMatrix[v_temp->nodes[best_c-1]][v_temp->nodes[best_c + 1]]
    //           << " - " << distanceMatrix[v_temp->nodes[best_c-1]][v_temp->nodes[best_c]]
    //           << " - " << distanceMatrix[v_temp->nodes[best_c]][v_temp->nodes[best_c + 1]]
    //           << std::endl;
    // std::cout << "Cost 1 " << v_temp->cost + v_temp_2->cost << std::endl;
    // std::cout << "Attepting to insert to get order " << v_temp_2->nodes[best_r] << " " << v_temp->nodes[best_c] << " " << v_temp_2->nodes[best_r+1] << std::endl;
    if(delta>-0.00001) break;
    else{
      int val_best_c = *(v_temp->nodes.begin()+best_c);
      // std::cout << val_best_c << std::endl;
      v_temp->nodes.erase(v_temp->nodes.begin()+best_c);
      v_temp->CalculateCost(distanceMatrix);
      if(v_temp->id == v_temp_2->id && best_c < best_r) v_temp_2->nodes.insert(v_temp_2->nodes.begin()+best_r, val_best_c);
      else v_temp_2->nodes.insert(v_temp_2->nodes.begin()+best_r+1, val_best_c);
      v_temp_2->CalculateCost(distanceMatrix);
      v_temp->load += nodes[val_best_c].demand;
      v_temp_2->load -= nodes[val_best_c].demand;
    }
    // std::cout << "Cost 2 " << v_temp->cost + v_temp_2->cost << std::endl;
    // v_temp->PrintStatus();
    // v_temp_2->PrintStatus();
  }

  // std::cout << "---------------------------" << std::endl;
  // std::cout << "Each vehicle's status/route after local search" << std::endl;
  // std::cout << "---------------------------" << std::endl;
  // for(auto& v:vehicles) v.PrintStatus();
  // for(auto& v:vehicles) v.PrintRoute();

  // std::cout << "Initial (greedy) cost: " << cost << std::endl;
  cost = 0;
  for(auto& v:vehicles) cost += v.cost;
  std::cout << "Cost: " << cost << std::endl;
  // for(auto& v:vehicles) v.PrintRoute();

  for(auto& i:nodes){
    if(!i.is_routed){
      std::cout << "Unreached node: " << std::endl;
      i.PrintStatus();
    }
  }
  // std::cout << "---------------------------" << std::endl;
  // std::cout << "End of LocalSearchIntra solution" << std::endl;
}
