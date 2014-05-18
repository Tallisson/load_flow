#include "load_flow.h"
#include <vector>
#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

int main(int argc, char ** argv) {
  LoadFlow * lf = new LoadFlow(5, 0.001);

  /*Bar b1(0, 1, 0, 0, SLACK, 0);
  Bar b2(0, 1, -0.3, 0.07, LOAD, 1);
  lf->AddBar(b1);
  lf->AddBar(b2);

  Node node(0.1923, -0.9415);
  lf->AssocBars(b1, b1, node);

  Node node1(-0.1923, 0.9615, 0.02);
  lf->AssocBars(b1, b2, node1);

  Node node2(0.1923, -0.9415);
  lf->AssocBars(b2, b2, node2);*/

  //lf->Execute();

  Bar b1(0, 1.06, 0, 0, SLACK, 0);
  Bar b2(0, 1.02, 0.2, 0, GENERATION, 1);
  Bar b3(0, 0, -0.45, -0.15, LOAD, 2);
  Bar b4(0, 0, -0.4, -0.05, LOAD, 3);
  Bar b5(0, 0, -0.6, -0.1, LOAD, 4);

  lf->AddBar(b1);
  lf->AddBar(b2);
  lf->AddBar(b3);
  lf->AddBar(b4);
  lf->AddBar(b5);

  /*Bar * b = lf->GetBar(1);
  cout << b->GetC() << endl;*/

  Admitt admmit1(0.02, 0.06, 0.0);
  Admitt admmit2(0.08, 0.24, 0.0);
  Admitt admmit3(0.06, 0.18, 0.0);
  Admitt admmit4(0.06, 0.18, 0.0);
  Admitt admmit5(0.04, 0.12, 0.0);
  Admitt admmit6(0.01, 0.03, 0.0);
  Admitt admmit7(0.08, 0.24, 0.0);

  lf->AssocBars(b1, b2, admmit1);
  lf->AssocBars(b1, b3, admmit2);
  lf->AssocBars(b2, b3, admmit3);
  lf->AssocBars(b2, b4, admmit4);
  lf->AssocBars(b2, b5, admmit5);
  lf->AssocBars(b3, b4, admmit6);
  lf->AssocBars(b4, b5, admmit7);

  Bar * bar;
  for(int i = 0; i < 3; i++) {
    bar = lf->GetBar(i);
    cout << "C: " << bar->GetC() << ", " << "S: " << bar->GetS() << endl;
  }
  /*Node node1(5, -15);
  Node node2(1.25, -3.75);
  Node node3(1.666, -5);
  Node node4(1.666, -5);
  Node node5(2.5, -7.5);
  Node node6(10, -30);
  Node node7(1.25, -3.75);

  lf->AssocBars(b1, b2, node1);
  lf->AssocBars(b1, b3, node2);
  lf->AssocBars(b2, b3, node3);
  lf->AssocBars(b2, b4, node4);
  lf->AssocBars(b2, b5, node5);
  lf->AssocBars(b3, b4, node6);
  lf->AssocBars(b4, b5, node7);*/

  /*for(int i = 0; i < 5; i++) {
    Bar * b = lf->GetBar(i);
    cout << "Bar(" << b->GetId()+1 << ") Conductance: " << b->GetC() << ", Susceptance: " << b->GetS() << endl;
  }*/
  lf->Execute();

  return 0;
}
