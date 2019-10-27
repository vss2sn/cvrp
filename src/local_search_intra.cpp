/**
* @file local_search_intra.cpp
* @author vss2sn
* @brief Contains the LocalSearchIntraSolution class (Local search restricted to within individual vehicles)
*/

#include "local_search_intra.hpp"

LocalSearchIntraSolution::LocalSearchIntraSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :Solution(nodes, vehicles, distanceMatrix){
  CreateInitialSolution();
} ;

LocalSearchIntraSolution::LocalSearchIntraSolution(Problem p)
  :Solution(p.nodes, p.vehicles, p.distanceMatrix){
  CreateInitialSolution();
};

LocalSearchIntraSolution::LocalSearchIntraSolution(Solution s)
  :Solution(s){
  CreateInitialSolution();
};

void LocalSearchIntraSolution::Solve(){
  double cost = 0;
  for(auto& v:vehicles) cost += v.cost;
  for(auto& v:vehicles){
    while(true){
      double delta = 0.0, cost_reduction, cost_increase;
      int cur, prev, next_c, rep, next_r, best_c, best_r;
      int  v_cur, v_prev, v_next_c, v_rep, v_next_r;
      for(cur=1;cur<v.nodes.size()-1;cur++){
        prev = cur-1;
        next_c = cur+1;

        v_cur = v.nodes[cur];
        v_prev = v.nodes[cur-1];
        v_next_c = v.nodes[cur+1];

        cost_reduction = distanceMatrix[v_prev][v_next_c]
                       - distanceMatrix[v_prev][v_cur]
                       - distanceMatrix[v_cur][v_next_c];
        for(rep=1;rep<v.nodes.size()-1;rep++){
          if(rep!=cur && rep!=cur-1){
            next_r = rep + 1;
            v_rep = v.nodes[rep];
            v_next_r = v.nodes[rep+1];

            cost_increase = distanceMatrix[v_rep][v_cur]
                          + distanceMatrix[v_cur][v_next_r]
                          - distanceMatrix[v_rep][v_next_r];
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
  std::cout << "Solution valid: " << CheckSolutionValid()<< std::endl;
}
