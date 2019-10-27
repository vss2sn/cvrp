/**
* @file tabu_search.cpp
* @author vss2sn
* @brief Contains the TabuSearchSolution class
*/

#include "tabu_search.hpp"

TabuSearchSolution::TabuSearchSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix, int n_tabu)
  :Solution(nodes, vehicles, distanceMatrix){
  this->n_tabu = n_tabu;
  CreateInitialSolution();
}

TabuSearchSolution::TabuSearchSolution(Problem p, int n_tabu)
  :Solution(p.nodes, p.vehicles, p.distanceMatrix){
  this->n_tabu = n_tabu;
  CreateInitialSolution();
}

TabuSearchSolution::TabuSearchSolution(Solution s, int n_tabu)
  :Solution(s){
  this->n_tabu = n_tabu;
}

inline bool TabuSearchSolution::IsTabu(const int& begin, const int& end){
  for(int i=begin; i<=end;i++){
    if(tabu_list_set.find(to_check[i])!=tabu_list_set.end()) return true;
  }
  return false;
}

inline bool TabuSearchSolution::Aspiration(double& cost_increase, double& cost_reduction){
  return new_cost + cost_increase + cost_reduction < best_cost;
}

void TabuSearchSolution::Solve(){
  double cost = 0;
  for(auto& v:vehicles) cost += v.cost;
  auto best_vehicles = vehicles;
  bool flag = false, flag2 = true;
  int max_it = 500, c_it = 0;
  int  cur, prev, next_c, rep, next_r, best_c=-1, best_r;
  int  v_cur, v_prev, v_next_c, v_rep, v_next_r;
  double delta = INT_MAX, cost_reduction, cost_increase, bcr, bci;
  best_cost = cost, new_cost = cost;
  Vehicle *v_temp_2, *v_temp;
  to_check = std::vector<std::vector<int>>(6,std::vector<int>(2,0));
  for(int i=0;i<n_tabu;i++) tabu_list_queue.push(std::vector<int>());
  while(c_it<max_it){
    ++c_it;
    delta = 1<<16;
    for(auto& v:vehicles){
      for(auto& v2:vehicles){
        for(cur=1;cur<v.nodes.size()-1;cur++){
          prev = cur-1;
          next_c = cur+1;

          v_cur = v.nodes[cur];
          v_prev = v.nodes[cur-1];
          v_next_c = v.nodes[cur+1];

          to_check[0][0] = v_prev;
          to_check[0][1] = v_cur;
          to_check[1][0] = v_cur;
          to_check[1][1] = v_prev;
          to_check[2][0] = v_cur;
          to_check[2][1] = v_next_c;
          to_check[3][0] = v_next_c;
          to_check[3][1] = v_cur;

          cost_reduction = distanceMatrix[v_prev][v_next_c]
                         - distanceMatrix[v_prev][v_cur]
                         - distanceMatrix[v_cur][v_next_c];

          to_check[4][1] = v_cur;
          to_check[5][0] = v_cur;

          for(rep=0;rep<v2.nodes.size()-1;rep++){
            v_rep = v2.nodes[rep];
            v_next_r = v2.nodes[rep+1];
            if(v_rep!=v_cur && (v.id!=v2.id || v_rep!=v_prev)){
              next_r = rep + 1;
              to_check[4][0] = v_rep;
              to_check[5][1] = v_next_r;

              cost_increase = distanceMatrix[v_rep][v_cur]
                            + distanceMatrix[v_cur][v_next_r]
                            - distanceMatrix[v_rep][v_next_r];
              if(cost_increase + cost_reduction < delta &&
                (v2.load - nodes[v_cur].demand >= 0 || v.id == v2.id) &&
                (!IsTabu(0,5) || Aspiration(cost_increase, cost_reduction))
                ){
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
    if(delta == 1<<16){
      std::cout << "No possible moves. Consider adjusting tabu list size." <<std::endl;
      break;
    }
    // TO check if solution found here is the same as the one found by the local search
    // if(delta>-0.00001) flag = true;
    // if(flag && flag2){
    //   std::cout << "Local search solution found. Cost: " << best_cost << " Iterations: " << c_it << std::endl;
    //   flag2  = false;
    // }
    int val_best_c = *(v_temp->nodes.begin()+best_c);
    v_temp->nodes.erase(v_temp->nodes.begin()+best_c);
    v_temp->CalculateCost(distanceMatrix);
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

    v_temp_2->CalculateCost(distanceMatrix);
    v_temp->load += nodes[val_best_c].demand;
    v_temp_2->load -= nodes[val_best_c].demand;

    new_cost = 0;
    for(auto& v:vehicles) new_cost+=v.cost;
    if(new_cost< best_cost){
      best_vehicles = vehicles;
      best_cost = new_cost;
    }
    tabu_list_set.erase(tabu_list_queue.front());
    tabu_list_queue.pop();
    tabu_list_set.erase(tabu_list_queue.front());
    tabu_list_queue.pop();
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
