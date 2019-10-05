#include "simulated_annealing.hpp"

inline bool SimulatedAnnealingSolution::AllowMove(double delta){//Vehicle *v1, Vehicle *v2, int cur, int rep){
  if(delta < -0.001) return true;
  else return false;
}

void SimulatedAnnealingSolution::Solve(){
  CreateInitialSolution();
  double cost = 0;
  for(auto& v:vehicles) cost += v.cost;
  // std::cout << "Init cost: "<< cost << std::endl;
  auto best_vehicles = vehicles;
  // std::cout << CheckSolutionValid() << std::endl;
  best_cost = cost;
  current_cost = cost;
  int stag_limit = 10000,stag;
  int n_vehicles = vehicles.size();
  for(int r=0;r<n_reheates;r++){
    // std::cout << "Reheat number: " << r << std::endl;
    stag = stag_limit;
    temp = max_temp;
    int cur, rep, prev, next_c, next_r;
    Vehicle *v1, *v2;
    double cost_increase, cost_reduction, delta;
    // usleep(5000000);
    while(--stag>=0){
      v1 = &vehicles[rand()%n_vehicles];
      v2 = &vehicles[rand()%n_vehicles];
      // std::cout << v1->nodes.size() << " " <<v2->nodes.size() << std::endl;
      if(v1->nodes.size()>2) cur = rand()%(v1->nodes.size()-2)+1; // do not select trailing zero or starting zero
      else continue;
      rep = rand()%(v2->nodes.size()-1);// do not select trailing zero
      if(v1->id==v2->id && (cur == rep+1 || cur == rep)) continue;
      prev = cur-1;
      next_c = cur+1;
      next_r = rep+1;
      cost_reduction = distanceMatrix[v1->nodes[prev]][v1->nodes[next_c]]
                     - distanceMatrix[v1->nodes[prev]][v1->nodes[cur]]
                     - distanceMatrix[v1->nodes[cur]][v1->nodes[next_c]];
      cost_increase = distanceMatrix[v2->nodes[rep]][v1->nodes[cur]]
                    + distanceMatrix[v1->nodes[cur]][v2->nodes[next_r]]
                    - distanceMatrix[v2->nodes[rep]][v2->nodes[next_r]];
      delta = cost_increase + cost_reduction;
      if((v2->load - nodes[v1->nodes[cur]].demand>=0 || v1->id==v2->id) && AllowMove(delta)){
        v1->load+=nodes[v1->nodes[cur]].demand;
        v2->load-=nodes[v1->nodes[cur]].demand;
        // std::cout << std::endl;
        // v1->PrintRoute();
        // std::cout << "v1->cost: "<<v1->cost << std::endl;
        // std::cout << v1->nodes[cur] << std::endl;
        // std::cout << "reduction "<<cost_reduction << std::endl;
        v1->cost += cost_reduction;
        // std::cout << "v1->cost: "<<v1->cost << std::endl;
        v2->cost += cost_increase;
        // std::cout << "v2->cost: "<<v2->cost << std::endl;

        int val = v1->nodes[cur];
        // std::cout << val << std::endl;
        // std::cout << v1->nodes.size() << std::endl;
        v1->nodes.erase(v1->nodes.begin()+cur);
        // std::cout << v1->nodes.size() << std::endl;

        if(v1->id==v2->id && cur < rep){
          v2->nodes.insert(v2->nodes.begin()+rep, val);
        }
        else{
          v2->nodes.insert(v2->nodes.begin()+rep+1, val);
        }
        current_cost += delta;
        // v1->PrintRoute();
        // usleep(1000000);

      }
      if(current_cost < best_cost){
        // std::cout << "Best solution iproved" << std::endl;
        best_vehicles[v1->id] = *v1;
        best_vehicles[v2->id] = *v2;
        best_cost = current_cost;
      }
    }
  }
  vehicles = best_vehicles;
  cost = 0;
  for(auto& v:vehicles){
    // std::cout << v.cost << std::endl;

    // v.CalculateCost(distanceMatrix);
    // std::cout << v.cost << std::endl;
    cost += v.cost;
  }
  std::cout << "Cost: " << cost << std::endl;
  for(auto& i:nodes){
    if(!i.is_routed){
      std::cout << "Unreached node: " << std::endl;
      i.PrintStatus();
    }
  }
  std::cout << "Solution valid: " << CheckSolutionValid()<< std::endl;
}
