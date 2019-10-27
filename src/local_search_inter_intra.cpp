/**
* @file local_search_inter_intra.cpp
* @author vss2sn
* @brief Contains the LocalSearchInterIntraSolution class (Local search extends to all vehicles)
*/

#include "local_search_inter_intra.hpp"

LocalSearchInterIntraSolution::LocalSearchInterIntraSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){
  CreateInitialSolution();
};

LocalSearchInterIntraSolution::LocalSearchInterIntraSolution(Problem p)
  :Solution(p.nodes, p.vehicles, p.distanceMatrix){
  CreateInitialSolution();
};

LocalSearchInterIntraSolution::LocalSearchInterIntraSolution(Solution s)
  :Solution(s){
    if(!s.CheckSolutionValid()){
      std::cout << "The input solution is invalid. Exiting." <<std::endl;
      exit(0);
    }
};

void LocalSearchInterIntraSolution::Solve(){
  double cost = 0;
  for(auto& v:vehicles) cost += v.cost;
  double delta = 0.0, cost_reduction, cost_increase, bcr, bci;
  int cur, prev, next_c, rep, next_r, best_c, best_r;
  int  v_cur, v_prev, v_next_c, v_rep, v_next_r;
  Vehicle *v_temp_2, *v_temp;
  while(true){
    delta = INT_MAX;
    for(auto& v:vehicles){
      for(auto& v2:vehicles){
        for(cur=1;cur<v.nodes.size()-1;cur++){
          prev = cur-1;
          next_c = cur+1;

          v_cur = v.nodes[cur];
          v_prev = v.nodes[cur-1];
          v_next_c = v.nodes[cur+1];

          cost_reduction = distanceMatrix[v_prev][v_next_c]
                         - distanceMatrix[v_prev][v_cur]
                         - distanceMatrix[v_cur][v_next_c];
          for(rep=0;rep<v2.nodes.size()-1;rep++){
            v_rep = v2.nodes[rep];
            v_next_r = v2.nodes[rep+1];
            if(v_rep!=v_cur && (v.id!=v2.id || v_rep!=v_prev)){
              next_r = rep + 1;
              cost_increase = distanceMatrix[v_rep][v_cur]
                            + distanceMatrix[v_cur][v_next_r]
                            - distanceMatrix[v_rep][v_next_r];
              if(cost_increase + cost_reduction < delta &&
                (v2.load - nodes[v_cur].demand >= 0 || v.id == v2.id)
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
    if(delta>-0.00001) break;
    else{
      int val_best_c = *(v_temp->nodes.begin()+best_c);
      v_temp->nodes.erase(v_temp->nodes.begin()+best_c);
      v_temp->CalculateCost(distanceMatrix);
      if(v_temp->id == v_temp_2->id && best_c < best_r) v_temp_2->nodes.insert(v_temp_2->nodes.begin()+best_r, val_best_c);
      else v_temp_2->nodes.insert(v_temp_2->nodes.begin()+best_r+1, val_best_c);
      v_temp_2->CalculateCost(distanceMatrix);
      v_temp->load += nodes[val_best_c].demand;
      v_temp_2->load -= nodes[val_best_c].demand;
    }
  }
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
