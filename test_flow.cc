#include "load_flow.h"
#include <vector>
#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

int main(int argc, char ** argv) {
  LoadFlow * lf = new LoadFlow(0.003);

  /*Bar * b1 = new Bar(0, 1, 0, 0, SLACK, 1, 0);
  Bar * b2 = new Bar(0, 0, -0.05, -0.02, LOAD, 2, 0);
  Bar * b3 = new Bar(0, 0.98, -0.15, 0, GENERATION, 3, 0);*/
  Bar * b1 = new Bar(0, 1, 0, 0, SLACK, 1, 0);
  Bar * b2 = new Bar(0, 1, -0.4, 0, GENERATION, 2, 0);

  lf->AddBar(b1);
  lf->AddBar(b2);

  Admitt * n1 = new Admitt(0.2, 1, 0.02);
  lf->AssocBars(b1, b2, n1);

  /*Bar * b1 = new Bar(0, 1.10, 0, 0, SLACK, 1, 0);
  Bar * b2 = new Bar(0, 1.10, 1.4, 0, GENERATION, 2, 0);
  Bar * b3 = new Bar(0, 1.10, 0.6, 0, GENERATION, 3, 0);
  Bar * b4 = new Bar(0, 0.0, -0.9, -0.6, LOAD, 4, 0);
  Bar * b5 = new Bar(0, 0.0, -1, -0.7, LOAD, 5, 0);
  Bar * b6 = new Bar(0, 0.0, -0.9, -0.6, LOAD, 6, 0);

  lf->AddBar(b1);
  lf->AddBar(b2);
  lf->AddBar(b3);
  lf->AddBar(b4);
  lf->AddBar(b5);
  lf->AddBar(b6);

  cout << "Testando Adição de Barras: " << endl;
  Bar * tmp;
  for(int i = 0; i < 6; i++) {
    tmp = lf->GetBar(i+1);
    cout << "Bar(" << tmp->GetId() << "), (a, v, p, q, t): " << tmp->GetAngle() << ", " << tmp->GetVoltage() << ", " <<
        tmp->GetAPower() << ", " << tmp->GetRPower() << ", " << tmp->GetType() << endl;
  }*/

  /*Node * n1 = new Node(1, 1);
  Node * n2 = new Node(2, 2, 1);
  Node * n3 = new Node(3, 3, -1);*/
  /*Admitt * n1 = new Admitt(0.10, 0.20, 0.04/2);
  Admitt * n2 = new Admitt(0.05, 0.20, 0.04/2);
  Admitt * n3 = new Admitt(0.08, 0.30, 0.06/2);
  Admitt * n4 = new Admitt(0.05, 0.25, 0.06/2);
  Admitt * n5 = new Admitt(0.05, 0.10, 0.02/2);
  Admitt * n6 = new Admitt(0.10, 0.30, 0.04/2);
  Admitt * n7 = new Admitt(0.07, 0.20, 0.05/2);
  Admitt * n8 = new Admitt(0.12, 0.26, 0.05/2);
  Admitt * n9 = new Admitt(0.02, 0.10, 0.02/2);
  Admitt * n10 = new Admitt(0.20, 0.40, 0.08/2);
  Admitt * n11 = new Admitt(0.10, 0.30, 0.06/2);


  lf->AssocBars(b1, b2, n1);
  lf->AssocBars(b1, b4, n2);
  lf->AssocBars(b1, b5, n3);
  lf->AssocBars(b2, b3, n4);
  lf->AssocBars(b2, b4, n5);
  lf->AssocBars(b2, b5, n6);
  lf->AssocBars(b2, b6, n7);
  lf->AssocBars(b3, b5, n8);
  lf->AssocBars(b3, b6, n9);
  lf->AssocBars(b4, b5, n10);
  lf->AssocBars(b5, b6, n11);*/

  /*cout << "Testando Adição de Ligação: " << endl;
  Node * n = lf->GetEdge(1, 2);
  cout << "Ligação(1,2) (c, s, sh): " << n->GetC() << ", " << n->GetS() << ", " << n->GetSh() << endl;

  n = lf->GetEdge(1, 3);
  cout << "Ligação(1,3) (c, s, sh): " << n->GetC() << ", " << n->GetS() << ", " << n->GetSh() << endl;

  n = lf->GetEdge(2, 3);
  cout << "Ligação(2,3) (c, s, sh): " << n->GetC() << ", " << n->GetS() << ", " << n->GetSh() << endl;

  n = lf->GetEdge(2, 4);
  cout << "Ligação(2,4) (c, s, sh): " << n->GetC() << ", " << n->GetS() << ", " << n->GetSh() << endl;

  n = lf->GetEdge(2, 5);
  cout << "Ligação(2,5) (c, s, sh): " << n->GetC() << ", " << n->GetS() << ", " << n->GetSh() << endl;

  n = lf->GetEdge(3, 4);
  cout << "Ligação(3,4) (c, s, sh): " << n->GetC() << ", " << n->GetS() << ", " << n->GetSh() << endl;

  n = lf->GetEdge(4, 5);
  cout << "Ligação(4,5) (c, s, sh): " << n->GetC() << ", " << n->GetS() << ", " << n->GetSh() << endl;*/

  lf->Execute();
  return 0;
}
