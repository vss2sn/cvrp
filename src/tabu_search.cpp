#include "tabu_search.hpp"

inline void TabuSearchSolution::CreateFirstArcSetToCheck(Vehicle& v, const int& cur){
  to_check[0][0] = v.nodes[cur-1];
  to_check[0][1] = v.nodes[cur];
  to_check[1][0] = v.nodes[cur];
  to_check[1][1] = v.nodes[cur-1];
  to_check[2][0] = v.nodes[cur];
  to_check[2][1] = v.nodes[cur+1];
  to_check[3][0] = v.nodes[cur+1];
  to_check[3][1] = v.nodes[cur];
  to_check[4][1] = v.nodes[cur];
  to_check[5][0] = v.nodes[cur];
}

inline void TabuSearchSolution::CreateSecondArcSetToCheck(Vehicle& v2, const int& rep){
  to_check[4][0] = v2.nodes[rep];
  to_check[5][1] = v2.nodes[rep+1];
}

inline bool TabuSearchSolution::CheckTabu(int const begin, int const end){
  for(int i=begin;i<=end;i++){
    if(tabu_list_set.find(to_check[i])!=tabu_list_set_end) return true;
  }
  return false;
}

void TabuSearchSolution::Solve(){

  CreateInitialSolution();
  for(auto& v:vehicles) cost += v.cost;
  best_vehicles = vehicles;
  best_cost = cost;
  new_cost = cost;
  tabu_list_set_end = tabu_list_set.end();
  while(c_it<max_it){
    ++c_it;
    delta = INT_MAX;
    best_c=-1;
    tabu_list_set_end = tabu_list_set.end();
    for(auto& v:vehicles){
      for(auto& v2:vehicles){
        for(cur=1;cur<v.nodes.size()-1;cur++){
          CreateFirstArcSetToCheck(v, cur);
          outer_tabu = CheckTabu(0,3);

          prev = cur-1;
          next_c = cur+1;
          cost_reduction = distanceMatrix[v.nodes[prev]][v.nodes[next_c]]
                         - distanceMatrix[v.nodes[prev]][v.nodes[cur]]
                         - distanceMatrix[v.nodes[cur]][v.nodes[next_c]];

          for(rep=0;rep<v2.nodes.size()-1;rep++){
            CreateSecondArcSetToCheck(v2, rep);
            inner_tabu = CheckTabu(4,5);

            if(v2.nodes[rep]!=v.nodes[cur] && (v.id!=v2.id || v2.nodes[rep]!=v.nodes[cur-1])){
              next_r = rep + 1;
              cost_increase = distanceMatrix[v2.nodes[rep]][v.nodes[cur]]
                            + distanceMatrix[v.nodes[cur]][v2.nodes[next_r]]
                            - distanceMatrix[v2.nodes[rep]][v2.nodes[next_r]];
              if((v2.load - nodes[v.nodes[cur]].demand >= 0 || v.id == v2.id)
                 && ((!outer_tabu && !inner_tabu && cost_increase + cost_reduction < delta)
                      || (new_cost + cost_increase + cost_reduction < best_cost))) {
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
    int val_best_c = *(v_temp->nodes.begin()+best_c);
    v_temp->nodes.erase(v_temp->nodes.begin()+best_c);

    v_temp->cost+=bcr;
    if(v_temp->id == v_temp_2->id && best_c < best_r){
      v_temp_2->nodes.insert(v_temp_2->nodes.begin()+best_r, val_best_c);
      tabu_list_set.insert({v_temp_2->nodes[best_r-1], val_best_c});
      tabu_list_queue.push({v_temp_2->nodes[best_r-1], val_best_c});
    }
    else{
      v_temp_2->nodes.insert(v_temp_2->nodes.begin()+best_r+1, val_best_c);
      tabu_list_set.insert({v_temp_2->nodes[best_r], val_best_c});
      tabu_list_queue.push({v_temp_2->nodes[best_r], val_best_c});
    }
    tabu_list_set.insert({v_temp->nodes[best_c-1], val_best_c});
    tabu_list_queue.push({v_temp->nodes[best_c-1], val_best_c});
    v_temp_2->cost +=bci;
    v_temp->load += nodes[val_best_c].demand;
    v_temp_2->load -= nodes[val_best_c].demand;

    new_cost = 0;
    for(auto& v:vehicles) new_cost+=v.cost;
    if(new_cost< best_cost){
      best_vehicles = vehicles;
      best_cost = new_cost;
    }
    if(tabu_list_set.size()> n_tabu){
      auto front = tabu_list_queue.front();
      tabu_list_set.erase(front);
      tabu_list_queue.pop();
      front = tabu_list_queue.front();
      tabu_list_set.erase(front);
      tabu_list_queue.pop();
    }
  }
  vehicles = best_vehicles;
  cost = 0;
  for(auto& v:vehicles) cost += v.cost;
  std::cout << "Cost: " << cost << std::endl;
  for(auto& i:nodes){
    if(!i.is_routed){
      std::cout << "Unreached node: " << std::endl;
      i.PrintStatus();
    }
  }
  std::cout << "Solution valid: " << CheckSolutionValid()<< std::endl;
}
