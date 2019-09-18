#include "ts.hpp"

void TabuSearchSolution::Solve(){

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
  int max_it = 10000, c_it = 0;
  auto best_vehicles = vehicles;
  double best_cost = cost;
  double new_cost = cost;
bool flag = false, flag2 = true;
  while(c_it<max_it){
    ++c_it;
    double delta = INT_MAX, cost_reduction, cost_increase, bcr, bci;
    int cur, prev, next_c, rep, next_r, best_c=-1, best_r;
    Vehicle *v_temp_2, *v_temp;
    for(auto& v:vehicles){
      for(auto& v2:vehicles){
        std::vector<std::vector<int>> to_check; // invert order of v1, v2 and cur, rep+1
        std::vector<int> ctmp;
        ctmp.push_back(0);
        ctmp.push_back(0);
        for(int i=0;i<4;i++) to_check.push_back(ctmp);
        for(cur=1;cur<v.nodes.size()-1;cur++){
          to_check[0][0] = v.nodes[cur-1];
          to_check[0][1] = v.nodes[cur];
          to_check[1][0] = v.nodes[cur];
          to_check[1][1] = v.nodes[cur-1];
          to_check[2][0] = v.nodes[cur];
          to_check[2][1] = v.nodes[cur+1];
          to_check[3][0] = v.nodes[cur+1];
          to_check[3][1] = v.nodes[cur];

          //std::cout << to_check.size() << " " << to_check[0].size()<< std::endl;
          // for(auto&f:to_check) std::cout << f[0] << " " << f[1] << std::endl;
          if(tabu_list_set.find(to_check[0])!=tabu_list_set.end()) continue;
          else if(tabu_list_set.find(to_check[1])!=tabu_list_set.end()) continue;
          else if(tabu_list_set.find(to_check[2])!=tabu_list_set.end()) continue;
          else if(tabu_list_set.find(to_check[3])!=tabu_list_set.end()) continue;

          prev = cur-1;
          next_c = cur+1;
          cost_reduction = distanceMatrix[v.nodes[prev]][v.nodes[next_c]]
                         - distanceMatrix[v.nodes[prev]][v.nodes[cur]]
                         - distanceMatrix[v.nodes[cur]][v.nodes[next_c]];
          for(rep=0;rep<v2.nodes.size()-1;rep++){
            to_check[0][0] = v2.nodes[rep];
            to_check[0][1] = v.nodes[cur];
            to_check[1][0] = v.nodes[cur];
            to_check[1][1] = v2.nodes[rep+1];

            if(tabu_list_set.find(to_check[0])!=tabu_list_set.end()) continue;
            else if(tabu_list_set.find(to_check[1])!=tabu_list_set.end()) continue;

            if(v2.nodes[rep]!=v.nodes[cur] && (v.id!=v2.id || v2.nodes[rep]!=v.nodes[cur-1])){
              next_r = rep + 1;
              cost_increase = distanceMatrix[v2.nodes[rep]][v.nodes[cur]]
                            + distanceMatrix[v.nodes[cur]][v2.nodes[next_r]]
                            - distanceMatrix[v2.nodes[rep]][v2.nodes[next_r]];
              if(cost_increase + cost_reduction < delta && (v2.load - nodes[v.nodes[cur]].demand >= 0
              || v.id == v2.id) //&&
              //(tabu_list_set.find(to_check)==tabu_list_set.end() || new_cost + cost_increase + cost_reduction < best_cost)
              ){
                // if(new_cost + cost_increase + cost_reduction < best_cost) std::cout << "Aspiration" << std::endl;
                  bci = cost_increase;
                  bcr = cost_reduction;
                  delta = cost_increase + cost_reduction;
                  best_c = cur;
                  best_r = rep;
                  v_temp_2 = &v2;
                  v_temp = &v;
              }
              // if(tabu_list_set.find(to_check)!=tabu_list_set.end()){
              //   std::cout << "Tabu found already" << std::endl;
              //   std::cout << "Min cost: " << best_cost << std::endl;
              // }
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
    if(delta>-0.00001) flag = true;
    if(flag && flag2){
      // std::cout << "Reached lsii soln" << std::endl;
      flag2  = false;
    }
    // else{
    if(best_c == -1) break;//continue;
      int val_best_c = *(v_temp->nodes.begin()+best_c);
      // std::cout << val_best_c << std::endl;
      // std::cout << c_it<< ": "<<delta << std::endl;

      v_temp->nodes.erase(v_temp->nodes.begin()+best_c);
      v_temp->CalculateCost(distanceMatrix);
      if(v_temp->id == v_temp_2->id && best_c < best_r){
        v_temp_2->nodes.insert(v_temp_2->nodes.begin()+best_r, val_best_c);
        if(!flag2){
          // std::cout << "Added to tabu" << std::endl;
          tabu_list_set.insert({v_temp->nodes[best_c-1], val_best_c});//val_best_c});
          tabu_list_queue.push({v_temp->nodes[best_c-1], val_best_c});//val_best_c});
          tabu_list_set.insert({v_temp_2->nodes[best_r-1], val_best_c});//val_best_c});
          tabu_list_queue.push({v_temp_2->nodes[best_r-1], val_best_c});//val_best_c});
          // std::cout << v_temp->nodes[best_c-1] << " " <<val_best_c << std::endl;
          // std::cout << v_temp_2->nodes[best_r-2] <<" " <<val_best_c << " " << std::endl;
        }


      }
      else{
        v_temp_2->nodes.insert(v_temp_2->nodes.begin()+best_r+1, val_best_c);
        if(!flag2){
          // std::cout << "Added to tabu" << std::endl;
          tabu_list_set.insert({v_temp->nodes[best_c-1], val_best_c});//val_best_c});
          tabu_list_queue.push({v_temp->nodes[best_c-1], val_best_c});//val_best_c});
          tabu_list_set.insert({v_temp_2->nodes[best_r], val_best_c});//val_best_c});
          tabu_list_queue.push({v_temp_2->nodes[best_r], val_best_c});//val_best_c});
          // std::cout << v_temp->nodes[best_c-1] << " " <<val_best_c << std::endl;
          // std::cout << v_temp_2->nodes[best_r-2] << " " <<val_best_c << " " << std::endl;
        }
      }
      v_temp_2->CalculateCost(distanceMatrix);
      v_temp->load += nodes[val_best_c].demand;
      v_temp_2->load -= nodes[val_best_c].demand;

      new_cost = 0;
      for(auto& v:vehicles) new_cost+=v.cost;
      if(new_cost< best_cost){
        // std::cout << "Update" << std::endl;
        best_vehicles = vehicles;
        best_cost = new_cost;
      }
      // else if(new_cost == best_cost) std::cout << "Reached best cost again"<< std::endl;
      if(tabu_list_set.size()>50){

        // for(int i=0;i<25;i++){
          auto front = tabu_list_queue.front();
          tabu_list_set.erase(front);
          tabu_list_queue.pop();
        // }

        front = tabu_list_queue.front();
        tabu_list_set.erase(front);
        tabu_list_queue.pop();
      }
    // }
    // std::cout << "Cost 2 " << v_temp->cost + v_temp_2->cost << std::endl;
    // v_temp->PrintStatus();
    // v_temp_2->PrintStatus();
  }
  vehicles = best_vehicles;

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
