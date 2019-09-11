#include "genetic_algorithm.hpp"
#include "greedy.hpp"
#include "local_search_intra.hpp"
#include "local_search_inter_intra.hpp"

int main(){
  Problem p;

  GreedySolution vrp_greedy(p);
  vrp_greedy.Solve();

  LocalSearchIntraSolution vrp_lsi(p);
  vrp_lsi.Solve();

  LocalSearchInterIntraSolution vrp_lsii(p);
  vrp_lsii.Solve();

  GeneticAlgorithmSolution vrp_genetic(p, 30, 10000);
  vrp_genetic.Solve();
}
