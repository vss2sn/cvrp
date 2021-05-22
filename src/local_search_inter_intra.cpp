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
  :Solution(p.nodes_, p.vehicles_, p.distanceMatrix_){
  CreateInitialSolution();
};

LocalSearchInterIntraSolution::LocalSearchInterIntraSolution(Solution s)
  :Solution(s){
    if(!s.CheckSolutionValid()){
      std::cout << "The input solution is invalid. Exiting." <<'\n';
      exit(0);
    }
};

void LocalSearchInterIntraSolution::Solve(){
  double cost = 0;
  for(const auto& v:vehicles_) cost += v.cost_;
  double delta = 0.0, cost_reduction, cost_increase;
  int best_c = -1, best_r = -1;
  size_t cur, rep;
  int v_cur, v_prev, v_next_c, v_rep, v_next_r;
  Vehicle *v_temp_2 = nullptr, *v_temp = nullptr;
  while(true){
    delta = INT_MAX;
    for(auto& v:vehicles_){
      for(cur=1;cur<v.nodes_.size()-1;cur++){
        v_cur = v.nodes_[cur];
        v_prev = v.nodes_[cur-1];
        v_next_c = v.nodes_[cur+1];

        cost_reduction = distanceMatrix_[v_prev][v_next_c]
                       - distanceMatrix_[v_prev][v_cur]
                       - distanceMatrix_[v_cur][v_next_c];
        for(auto& v2:vehicles_){
          for(rep=0;rep<v2.nodes_.size()-1;rep++){
            v_rep = v2.nodes_[rep];
            v_next_r = v2.nodes_[rep+1];
            if(v_rep!=v_cur && (v.id_!=v2.id_ || v_rep!=v_prev)){
              cost_increase = distanceMatrix_[v_rep][v_cur]
                            + distanceMatrix_[v_cur][v_next_r]
                            - distanceMatrix_[v_rep][v_next_r];
              if(cost_increase + cost_reduction < delta &&
                (v2.load_ - nodes_[v_cur].demand_ >= 0 || v.id_ == v2.id_)
              ){
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
      int val_best_c = *(v_temp->nodes_.begin()+best_c);
      v_temp->nodes_.erase(v_temp->nodes_.begin()+best_c);
      v_temp->CalculateCost(distanceMatrix_);
      if(v_temp->id_ == v_temp_2->id_ && best_c < best_r) v_temp_2->nodes_.insert(v_temp_2->nodes_.begin()+best_r, val_best_c);
      else v_temp_2->nodes_.insert(v_temp_2->nodes_.begin()+best_r+1, val_best_c);
      v_temp_2->CalculateCost(distanceMatrix_);
      v_temp->load_ += nodes_[val_best_c].demand_;
      v_temp_2->load_ -= nodes_[val_best_c].demand_;
    }
  }
  cost = 0;
  for(const auto& v:vehicles_) cost += v.cost_;
  std::cout << "Cost: " << cost << '\n';
  for(const auto& i:nodes_){
    if(!i.is_routed_){
      std::cout << "Unreached node: " << '\n';
      std::cout << i << '\n';
    }
  }
  std::cout << "Solution valid: " << CheckSolutionValid()<< '\n';
}
