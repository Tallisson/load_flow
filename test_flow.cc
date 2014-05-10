#include "load_flow.h"
#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char ** argv) {
  LoadFlow * lf = new LoadFlow(2, 0.003);

  Bar b1(0, 1, 0, 0, SLACK, 0);
  Bar b2(0, 1, -0.4, 0.0, GENERATION, 1);
  lf->AddBar(b1);
  lf->AddBar(b2);

  Node node(0.1923, -0.9415);
  lf->AssocBars(b1, b1, node);

  Node node1(-0.1923, 0.9615);
  lf->AssocBars(b1, b2, node1);

  Node node2(0.1923, -0.9415);
  lf->AssocBars(b2, b2, node2);

  lf->Execute();

  /*Bar b1(0, 1.06, 0, 0, SLACK, 0);
  Bar b2(0, 1.02, 0.2, 0, GENERATION, 1);
  Bar b3(0, 0, -0.45, -0.15, LOAD, 2);
  Bar b4(0, 0, -0.4, -0.05, LOAD, 3);
  Bar b5(0, 0, -0.6, -0.1, LOAD, 4);

  lf->AddBar(b1);
  lf->AddBar(b2);
  lf->AddBar(b3);
  lf->AddBar(b4);
  lf->AddBar(b5);

  Bar * b = lf->GetBar(1);
  cout << b->GetAngle() << ", " << b->GetVoltage() << ", " << b->GetAPower() << ", " << b->GetRPower() << ", " << b->GetType() << ", " << b->GetId() << endl;

  Node node1(5, -15);
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
  lf->AssocBars(b4, b5, node7);

  lf->Execute();*/
}
