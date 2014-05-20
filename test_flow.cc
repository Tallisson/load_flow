#include "load_flow.h"
//#include "graph.h"
#include <vector>
#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

int main(int argc, char ** argv) {
  //Graph * g = new Graph();
  LoadFlow * lf = new LoadFlow(3);
  /*Bar * b1 = new Bar(0, 1, 0, 0, SLACK, 1, 0);
  Bar * b2 = new Bar(0, 0, -0.05, -0.02, LOAD, 2, 0);
  Bar * b3 = new Bar(0, 0.98, -0.15, 0, GENERATION, 3, 0);*/


  Bar * b1 = new Bar(0, 1.06, 0, 0, SLACK, 1, 0);
  Bar * b2 = new Bar(0, 1.00, 0.2, 0, GENERATION, 2, 0);
  Bar * b3 = new Bar(0, 0, -0.45, -0.15, LOAD, 3, 0);
  Bar * b4 = new Bar(0, 0, -0.4, -0.05, LOAD, 4, 0);
  Bar * b5 = new Bar(0, 0, -0.6, -0.1, LOAD, 5, 0);

  lf->AddBar(b1);
  lf->AddBar(b2);
  lf->AddBar(b3);
  lf->AddBar(b4);
  lf->AddBar(b5);

  cout << "Testando Adição de Barras: " << endl;
  Bar * tmp;
  for(int i = 0; i < 5; i++) {
    tmp = lf->GetBar(i+1);
    cout << "Bar(" << tmp->GetId() << "), (a, v, p, q, t): " << tmp->GetAngle() << ", " << tmp->GetVoltage() << ", " <<
        tmp->GetAPower() << ", " << tmp->GetRPower() << ", " << tmp->GetType() << endl;
  }

  /*Node * n1 = new Node(1, 1);
  Node * n2 = new Node(2, 2, 1);
  Node * n3 = new Node(3, 3, -1);*/
  Admitt * n1 = new Admitt(0.02, 0.06, 0.03);
  Admitt * n2 = new Admitt(0.08, 0.24, 0.025);
  Admitt * n3 = new Admitt(0.06, 0.18, 0.02);
  Admitt * n4 = new Admitt(0.06, 0.18, 0.02);
  Admitt * n5 = new Admitt(0.04, 0.12, 0.015);
  Admitt * n6 = new Admitt(0.01, 0.03, 0.01);
  Admitt * n7 = new Admitt(0.08, 0.24, 0.025);

  lf->AssocBars(b1, b2, n1);
  lf->AssocBars(b1, b3, n2);
  lf->AssocBars(b2, b3, n3);
  lf->AssocBars(b2, b4, n4);
  lf->AssocBars(b2, b5, n5);
  lf->AssocBars(b3, b4, n6);
  lf->AssocBars(b4, b5, n7);

  cout << "Testando Adição de Ligação: " << endl;
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
  cout << "Ligação(4,5) (c, s, sh): " << n->GetC() << ", " << n->GetS() << ", " << n->GetSh() << endl;

  lf->Execute();
  return 0;
}
