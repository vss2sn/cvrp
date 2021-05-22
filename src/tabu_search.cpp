/**
* @file tabu_search.cpp
* @author vss2sn
* @brief Contains the TabuSearchSolution class
*/

#include "tabu_search.hpp"

TabuSearchSolution::TabuSearchSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix, int n_tabu)
  :Solution(nodes, vehicles, distanceMatrix){
  this->n_tabu_ = n_tabu;
  CreateInitialSolution();
}

TabuSearchSolution::TabuSearchSolution(Problem p, int n_tabu)
  :Solution(p.nodes_, p.vehicles_, p.distanceMatrix_){
  this->n_tabu_ = n_tabu;
  CreateInitialSolution();
}

TabuSearchSolution::TabuSearchSolution(Solution s, int n_tabu)
  :Solution(s){
  this->n_tabu_ = n_tabu;
  if(!s.CheckSolutionValid()){
    std::cout << "The input solution is invalid. Exiting." <<'\n';
    exit(0);
  }
}

inline bool TabuSearchSolution::IsTabu(const int begin, const int end){
  for(int i=begin; i<=end;i++){
    if(tabu_list_set_.find(to_check_[i])!=tabu_list_set_.end()) return true;
  }
  return false;
}

inline bool TabuSearchSolution::Aspiration(double cost_increase, double cost_reduction){
  return new_cost_ + cost_increase + cost_reduction < best_cost_;
}

void TabuSearchSolution::Solve(){
  double cost = 0;
  for(auto& v:vehicles_) cost += v.cost_;
  auto best_vehicles = vehicles_;
  // bool flag = false, flag2 = true;
  int max_it = 500, c_it = 0;
  // int  cur, prev, next_c, rep, next_r, best_c=-1, best_r;
  int best_c = -1, best_r = -1;
  size_t cur, rep;
  int  v_cur, v_prev, v_next_c, v_rep, v_next_r;
  double delta = INT_MAX, cost_reduction, cost_increase;//, bcr, bci;
  best_cost_ = cost, new_cost_ = cost;
  Vehicle *v_temp_2 = nullptr, *v_temp = nullptr;
  to_check_ = std::vector<std::vector<int>>(6,std::vector<int>(2,0));
  for(int i=0;i<n_tabu_;i++) tabu_list_queue_.push(std::vector<int>());
  while(c_it<max_it){
    ++c_it;
    delta = 1<<16;
    for(auto& v:vehicles_){
      for(cur=1;cur<v.nodes_.size()-1;cur++){
        // prev = cur-1;
        // next_c = cur+1;

        v_cur = v.nodes_[cur];
        v_prev = v.nodes_[cur-1];
        v_next_c = v.nodes_[cur+1];

        to_check_[0][0] = v_prev;
        to_check_[0][1] = v_cur;
        to_check_[1][0] = v_cur;
        to_check_[1][1] = v_prev;
        to_check_[2][0] = v_cur;
        to_check_[2][1] = v_next_c;
        to_check_[3][0] = v_next_c;
        to_check_[3][1] = v_cur;

        cost_reduction = distanceMatrix_[v_prev][v_next_c]
                       - distanceMatrix_[v_prev][v_cur]
                       - distanceMatrix_[v_cur][v_next_c];

        to_check_[4][1] = v_cur;
        to_check_[5][0] = v_cur;

        for(auto& v2:vehicles_){
          for(rep=0;rep<v2.nodes_.size()-1;rep++){
            v_rep = v2.nodes_[rep];
            v_next_r = v2.nodes_[rep+1];
            if(v_rep!=v_cur && (v.id_!=v2.id_ || v_rep!=v_prev)){
              to_check_[4][0] = v_rep;
              to_check_[5][1] = v_next_r;

              cost_increase = distanceMatrix_[v_rep][v_cur]
                            + distanceMatrix_[v_cur][v_next_r]
                            - distanceMatrix_[v_rep][v_next_r];
              if(cost_increase + cost_reduction < delta &&
                (v2.load_ - nodes_[v_cur].demand_ >= 0 || v.id_ == v2.id_) &&
                (!IsTabu(0,5) || Aspiration(cost_increase, cost_reduction))
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
    if(delta == 1<<16){
      std::cout << "No possible moves. Consider adjusting tabu list size." <<'\n';
      break;
    }
    // TO check if solution found here is the same as the one found by the local search
    // if(delta>-0.00001) flag = true;
    // if(flag && flag2){
    //   std::cout << "Local search solution found. Cost: " << best_cost_ << " Iterations: " << c_it << '\n';
    //   flag2  = false;
    // }
    int val_best_c = *(v_temp->nodes_.begin()+best_c);
    v_temp->nodes_.erase(v_temp->nodes_.begin()+best_c);
    v_temp->CalculateCost(distanceMatrix_);
    if(v_temp->id_ == v_temp_2->id_ && best_c < best_r){
      v_temp_2->nodes_.insert(v_temp_2->nodes_.begin()+best_r, val_best_c);
      tabu_list_set_.insert({v_temp_2->nodes_[best_r-1], val_best_c});
      tabu_list_queue_.push({v_temp_2->nodes_[best_r-1], val_best_c});
    }
    else{
      v_temp_2->nodes_.insert(v_temp_2->nodes_.begin()+best_r+1, val_best_c);
      tabu_list_set_.insert({v_temp_2->nodes_[best_r], val_best_c});
      tabu_list_queue_.push({v_temp_2->nodes_[best_r], val_best_c});
    }
    tabu_list_set_.insert({v_temp->nodes_[best_c-1], val_best_c});
    tabu_list_queue_.push({v_temp->nodes_[best_c-1], val_best_c});

    v_temp_2->CalculateCost(distanceMatrix_);
    v_temp->load_ += nodes_[val_best_c].demand_;
    v_temp_2->load_ -= nodes_[val_best_c].demand_;

    new_cost_ = 0;
    for(auto& v:vehicles_) new_cost_+=v.cost_;

    if(new_cost_< best_cost_){
      best_vehicles = vehicles_;
      best_cost_ = new_cost_;
    }
    tabu_list_set_.erase(tabu_list_queue_.front());
    tabu_list_queue_.pop();
    tabu_list_set_.erase(tabu_list_queue_.front());
    tabu_list_queue_.pop();
  }
  vehicles_ = best_vehicles;
  cost = 0;
  for(auto& v:vehicles_) cost += v.cost_;
  std::cout << "Cost: " << cost << '\n';
  for(auto& i:nodes_){
    if(!i.is_routed_){
      std::cout << "Unreached node: " << '\n';
      std::cout << i << '\n';
    }
  }
  std::cout << "Solution valid: " << CheckSolutionValid()<< '\n';
}
