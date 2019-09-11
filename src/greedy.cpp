#include "greedy.hpp"

void GreedySolution::Solve(){
  std::cout << "Greedy Solution" << std::endl;
  std::cout << "---------------" << std::endl;
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
        v.nodes.push_back(depot.id);
        break;
      }
    }
    v.PrintStatus();
  }

  for(auto& i:nodes){
    if(!i.is_routed){
      std::cout << "Unreached node: " << std::endl;
      i.PrintStatus();
    }
  }
  std::cout << "---------------------------" << std::endl;
  std::cout << "End of greedy solution" << std::endl;
}

int main(){

  std::random_device rd; // obtain a random number from hardware
  std::mt19937 eng(rd()); // seed the generator
  std::uniform_int_distribution<int> ran(0,100); // define the range

  int noc = 5;
	int nov = 3;

  Node depot(50, 50, 0, 0, true);

  std::vector<Node> nodes;
  nodes.push_back(depot);

  for(int i = 1; i <=noc; ++i) nodes.emplace_back(ran(eng), ran(eng), i, ran(eng), false);

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


  std::vector<Vehicle> vehicles;
  int load = 100, capacity = 100;
  for(int i=0; i<nov; ++i){
    vehicles.emplace_back(i, load, capacity);
    vehicles[i].nodes.push_back(0);
  }

  Problem p;
  distanceMatrix = p.distanceMatrix;
  vehicles = p.vehicles;
  nodes = p.nodes;

  GreedySolution g(p.nodes, p.vehicles, p.distanceMatrix);
  g.Solve();

  return 0;
}
