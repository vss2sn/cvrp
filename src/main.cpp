#include "main.hpp"

int main(){
  Problem p;

  GreedySolution vrp_greedy(p);
  vrp_greedy.Solve();

  LocalSearchIntraSolution vrp_lsi(p);
  vrp_lsi.Solve();

  LocalSearchInterIntraSolution vrp_lsii(p);
  vrp_lsii.Solve();

  GASolution vrp_ga(p, 100, 5000);
  vrp_ga.Solve();
}
