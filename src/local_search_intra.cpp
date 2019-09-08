#include "local_search_intra.hpp"

void LocalSearchIntraSolution::Solve(){
  std::cout << "LocalSearchIntra Solution" << std::endl;
  std::cout << "---------------" << std::endl;

  //Initial greedy search
  for(auto& v:vehicles){
    while(true){
      Node closest_node = find_closest(v, distanceMatrix, nodes);
      if(closest_node.id!=-1 && v.load - closest_node.demand >=0){
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
  std::cout << "---------------------------" << std::endl;
  std::cout << "Each vehicle's status before local search" << std::endl;
  std::cout << "---------------------------" << std::endl;
  for(auto& v:vehicles) v.PrintStatus();
  // Local Search
  // For every vehicle
  for(auto& v:vehicles){
    //loop continuously until no change detected
    while(true){
      double delta = 0.0, cost_reduction, cost_increase;
      int cur, prev, next_c, rep, next_r, best_c, best_r;
      for(cur=1;cur<v.nodes.size()-1;cur++){
        prev = cur-1;
        next_c = cur+1;
        cost_reduction = distanceMatrix[v.nodes[prev]][v.nodes[next_c]]
                       - distanceMatrix[v.nodes[prev]][v.nodes[cur]]
                       - distanceMatrix[v.nodes[cur]][v.nodes[next_c]];
        for(rep=1;rep<v.nodes.size()-1;rep++){
          if(rep!=cur && rep!=cur-1){
            next_r = rep + 1;
            cost_increase = distanceMatrix[v.nodes[rep]][v.nodes[cur]]
                          + distanceMatrix[v.nodes[cur]][v.nodes[next_r]]
                          - distanceMatrix[v.nodes[rep]][v.nodes[next_r]];
            if(cost_increase + cost_reduction < delta){
                delta = cost_increase + cost_reduction;
                best_c = cur;
                best_r = rep;
            }
          }
        }
      }
      if(delta==0) break;
      else{
        int val_best_c = *(v.nodes.begin()+best_c);
        v.nodes.erase(v.nodes.begin()+best_c);
        if(best_c < best_r) v.nodes.insert(v.nodes.begin()+best_r, val_best_c);
        else v.nodes.insert(v.nodes.begin()+best_r+1, val_best_c);
        v.CalculateCost(distanceMatrix);
      }
    }
  }
  std::cout << "---------------------------" << std::endl;
  std::cout << "Each vehicle's status/route after local search" << std::endl;
  std::cout << "---------------------------" << std::endl;
  // for(auto& v:vehicles) v.PrintStatus();
  for(auto& v:vehicles) v.PrintRoute();

  for(auto& i:nodes){
    if(!i.is_routed){
      std::cout << "Unreached node: " << std::endl;
      i.PrintStatus();
    }
  }
  std::cout << "---------------------------" << std::endl;
  std::cout << "End of LocalSearchIntra solution" << std::endl;
}


int main(){

  int range = 100;
  std::random_device rd; // obtain a random number from hardware
  std::mt19937 eng(rd()); // seed the generator
  std::uniform_int_distribution<int> ran(0,range); // define the range

  int noc = 5;
	int nov = 1;

  Node depot(0, 0, 0, 0, true);

  std::vector<Node> nodes;
  nodes.push_back(depot);

  for(int i = 1; i <=noc; ++i){
  nodes.emplace_back(ran(eng)-range/2, ran(eng)-range/2, i, ran(eng)/5, false);
    // nodes[i].PrintStatus();
  }

  std::vector<std::vector<double>> distanceMatrix;
  std::vector<double> tmp(nodes.size());
  for(int i=0; i<nodes.size(); ++i) distanceMatrix.push_back(tmp);
  for(int i=0; i<nodes.size(); ++i){
    for(int j=i; j < nodes.size(); ++j){
      distanceMatrix[i][j] = sqrt(double(pow((nodes[i].x - nodes[j].x),2)
                                       + pow((nodes[i].y - nodes[j].y),2)));
      distanceMatrix[j][i] = distanceMatrix[i][j];
    }
  }


  for(int i=0; i<nodes.size(); ++i){
    for(int j=0; j < nodes.size(); ++j){
      if(distanceMatrix[j][i] != distanceMatrix[i][j]) return 0;
    }
  }

  std::vector<Vehicle> vehicles;
  int load = 100, capacity = 100;
  for(int i=0; i<nov; ++i){
    vehicles.emplace_back(i, load, capacity);
    vehicles[i].nodes.push_back(0);
  }

  LocalSearchIntraSolution g(nodes, vehicles, distanceMatrix);
  g.Solve();

  return 0;
}
