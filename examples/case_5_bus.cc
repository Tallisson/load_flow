#include "../load_flow.h"
#include <vector>
#include <iostream>
#include <armadillo>

void exec5();

using namespace std;
using namespace arma;

void exec5() {
  LoadFlow * lf = new LoadFlow(0.0001);

//  V  = [ 1.06  1.02  0.00  0.00  0.00]';
//  a  = [ 0.00  0.00  0.00  0.00  0.00]';
//  Pg = [  0.0  40.0   0.0   0.0   0.0]'/Sbase;
//  Qg = [  0.0   0.0   0.0   0.0   0.0]'/Sbase;
//  Pc = [  0.0  20.0  45.0  40.0  60.0]'/Sbase;
//  Qc = [  0.0  10.0  15.0   5.0  10.0]'/Sbase;
//  Bsh = [ 0.0   0.0   0.0   0.0   0.0]'/Sbase;

  Bar * b1 = new Bar(0, 1.06, 0, 0, 0, 0, SLACK, 1, 0);
  Bar * b2 = new Bar(0, 1.02, 40, 20, 0, 10, GENERATION, 2, 0);
  Bar * b3 = new Bar(0, 0, 0, 45, 0, 15, LOAD, 3, 0);
  Bar * b4 = new Bar(0, 0, 0, 40, 0, 5, LOAD, 4, 0);
  Bar * b5 = new Bar(0, 0, 0, 60, 0, 10, LOAD, 5, 0);

  lf->AddBar(b1);
  lf->AddBar(b2);
  lf->AddBar(b3);
  lf->AddBar(b4);
  lf->AddBar(b5);

//  ni = [1 1 2 2 2 3 4]';
//  nf = [2 3 3 4 5 4 5]';
//  rkm    = [0.02 0.08 0.06 0.06 0.04 0.01 0.08];
//  xkm    = [0.06 0.24 0.18 0.18 0.12 0.03 0.24]';
//  bkm_sh = [0.06 0.05 0.04 0.04 0.03 0.02 0.05]'/2;

  Admitt * n1 = new Admitt(0.02, 0.06, 0.06/2);
  Admitt * n2 = new Admitt(0.08, 0.24, 0.05/2);
  Admitt * n3 = new Admitt(0.06, 0.18, 0.04/2);
  Admitt * n4 = new Admitt(0.06, 0.18, 0.04/2);
  Admitt * n5 = new Admitt(0.04, 0.12, 0.03/2);
  Admitt * n6 = new Admitt(0.01, 0.03, 0.02/2);
  Admitt * n7 = new Admitt(0.08, 0.24, 0.05/2);

  lf->AssocBars(b1, b2, n1);
  lf->AssocBars(b1, b3, n2);
  lf->AssocBars(b2, b3, n3);
  lf->AssocBars(b2, b4, n4);
  lf->AssocBars(b2, b5, n5);
  lf->AssocBars(b3, b4, n6);
  lf->AssocBars(b4, b5, n7);

  lf->Execute();
  delete lf;
}

int main(int argc, char ** argv) {
  exec5();

  return 0;
}
