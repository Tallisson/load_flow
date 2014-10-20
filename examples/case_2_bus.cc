#include "../controller/load_flow.h"
#include <vector>
#include <iostream>
#include <armadillo>

void exec2();

using namespace std;
using namespace load;
using namespace arma;

/*void exec2() {
  LoadFlow * lf = new LoadFlow(0.0001);
  lf->SetUseBase(false);
  // Execução 2 barras
  // Criando e adicionando barras
  Bar * b1 = new Bar(0, 1, 0, 0, SLACK, 1, 0);
  Bar * b2 = new Bar(0, 1.0, -0.40, 0, GENERATION, 2, 0);

  lf->AddBar(b1);
  lf->AddBar(b2);

  // Criando associação e ligando barras
  Admitt * assoc = new Admitt(0.2, 1.0, 0.02);

  lf->AssocBars(b1, b2, assoc);

  lf->Execute();
  delete lf;
}*/

/*int main(int argc, char ** argv) {
  exec2();

  return 0;
}*/

