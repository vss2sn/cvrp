/**
* @file local_search_intra.cpp
* @author vss2sn
* @brief Contains the LocalSearchIntraSolution class (Local search restricted to within individual vehicles)
*/

#include "local_search_intra.hpp"

LocalSearchIntraSolution::LocalSearchIntraSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){
  CreateInitialSolution();
}

LocalSearchIntraSolution::LocalSearchIntraSolution(Problem p)
  :Solution(p.nodes_, p.vehicles_, p.distanceMatrix_){
  CreateInitialSolution();
}

LocalSearchIntraSolution::LocalSearchIntraSolution(Solution s)
  :Solution(s){
    if(!s.CheckSolutionValid()){
      std::cout << "The input solution is invalid. Exiting." <<'\n';
      exit(0);
    }
}

void LocalSearchIntraSolution::Solve(){
  double cost = 0;
  for(const auto& v:vehicles_) cost += v.cost_;
  for(auto& v:vehicles_){
    while(true){
      double delta = 0.0, cost_reduction, cost_increase;
      // int cur, prev, next_c, rep, next_r, best_c, best_r;
      int best_c = -1, best_r = -1;
      size_t cur, rep;
      int  v_cur, v_prev, v_next_c, v_rep, v_next_r;
      for(cur=1;cur<v.nodes_.size()-1;cur++){
        // prev = cur-1;
        // next_c = cur+1;

        v_cur = v.nodes_[cur];
        v_prev = v.nodes_[cur-1];
        v_next_c = v.nodes_[cur+1];

        cost_reduction = distanceMatrix_[v_prev][v_next_c]
                       - distanceMatrix_[v_prev][v_cur]
                       - distanceMatrix_[v_cur][v_next_c];
        for(rep=1;rep<v.nodes_.size()-1;rep++){
          if(rep!=cur && rep!=cur-1){
            // next_r = rep + 1;
            v_rep = v.nodes_[rep];
            v_next_r = v.nodes_[rep+1];

            cost_increase = distanceMatrix_[v_rep][v_cur]
                          + distanceMatrix_[v_cur][v_next_r]
                          - distanceMatrix_[v_rep][v_next_r];
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
        int val_best_c = *(v.nodes_.begin()+best_c);
        v.nodes_.erase(v.nodes_.begin()+best_c);
        if(best_c < best_r) v.nodes_.insert(v.nodes_.begin()+best_r, val_best_c);
        else v.nodes_.insert(v.nodes_.begin()+best_r+1, val_best_c);
        v.CalculateCost(distanceMatrix_);
      }
    }
  }
  cost = 0;
  for(auto &v:vehicles_) cost +=v.cost_;
  std::cout << "Cost: " << cost << '\n';
  for(const auto& i:nodes_){
    if(!i.is_routed_){
      std::cout << "Unreached node: " << '\n';
      std::cout << i << '\n';
    }
  }
  std::cout << "Solution valid: " << CheckSolutionValid()<< '\n';
}
