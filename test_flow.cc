#include "load_flow.h"
#include <vector>
#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

int main(int argc, char ** argv) {
  LoadFlow * lf = new LoadFlow(0.003);

  // Execução 2 barras
  // Criando e adicionando barras
  //Bar * b1 = new Bar(0, 1, 0, 0, SLACK, 1, 0);
  // Bar * b2 = new Bar(0, 1.0, -0.40, 0, GENERATION, 2, 0);

  //lf->AddBar(b1);
  //lf->AddBar(b2);

  // Criando associação e ligando barras
  //Admitt * assoc = new Admitt(0.2, 1.0, 0.02);

  Bar * b1 = new Bar(0, 1.060, 232.40, 0.0, -16.90, 0, SLACK, 1, 0);
  Bar * b2 = new Bar(0, 1.045,  40.00, 21.7, 42.4, 12.7, GENERATION, 2, 0);
  Bar * b3 = new Bar(0, 1.010, 0, 94.2, 23.4, 19.0, GENERATION, 3, 0);
  Bar * b4 = new Bar(0,     0, 0, 47.8, 0, -3.9, LOAD, 4, 0);
  Bar * b5 = new Bar(0,     0, 0, 7.6, 0, 1.6, LOAD, 5, 0);
  Bar * b6 = new Bar(0, 1.070, 0, 11.2, 12.2, 7.5, GENERATION, 6, 0);
  Bar * b7 = new Bar(0,     0, 0, 0, 0, 0, LOAD, 7, 0);
  Bar * b8 = new Bar(0, 1.090, 0, 0, 17.4, 0, GENERATION, 8, 0);
  Bar * b9 = new Bar(0,     0, 0, 29.5, 0, 16.6, LOAD, 9, 0.19);
  Bar * b10 = new Bar(0,    0, 0, 9, 0, 5.8, LOAD, 10, 0);
  Bar * b11 = new Bar(0,    0, 0, 3.5, 0, 1.8, LOAD, 11, 0);
  Bar * b12 = new Bar(0,    0, 0, 6.1, 0, 1.6, LOAD, 12, 0);
  Bar * b13 = new Bar(0,    0, 0, 13.5, 0, 5.8, LOAD, 13, 0);
  Bar * b14 = new Bar(0,    0, 0, 14.9, 0, 5.0, LOAD, 14, 0);

  lf->AddBar(b1);
  lf->AddBar(b2);
  lf->AddBar(b3);
  lf->AddBar(b4);
  lf->AddBar(b5);
  lf->AddBar(b6);
  lf->AddBar(b7);
  lf->AddBar(b8);
  lf->AddBar(b9);
  lf->AddBar(b10);
  lf->AddBar(b11);
  lf->AddBar(b12);
  lf->AddBar(b13);
  lf->AddBar(b14);

  //rkm    = [0.01938  0.05403  0.04699  0.05811  0.05695  0.06701  0.01335   0.0      0.0      0.0     0.09498   0.12291  0.06615  0.0     0.0      0.03181  0.12711  0.08205  0.22092  0.17093];
  //xkm    = [0.05917  0.22304  0.19797  0.17632  0.17388  0.17103  0.04211  0.20912  0.55618  0.25202  0.19890  0.25581  0.13027  0.17615  0.11001  0.08450  0.27038  0.19207  0.19988  0.34802]';
  //bkm_sh = [0.0528   0.0492   0.0438   0.0340   0.0346   0.0128   0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0]'/2;

  Admitt * a1 = new Admitt(0.01938, 0.05917, 0.0528/2);
  Admitt * a2 = new Admitt(0.05403, 0.22304, 0.0492/2);
  Admitt * a3 = new Admitt(0.04699, 0.19797, 0.0438/2);
  Admitt * a4 = new Admitt(0.05811, 0.17632, 0.0340/2);
  Admitt * a5 = new Admitt(0.05695, 0.17388, 0.0346/2);
  Admitt * a6 = new Admitt(0.06701, 0.17103, 0.0128/2);
  Admitt * a7 = new Admitt(0.01335, 0.04211, 0);
  Admitt * a8 = new Admitt(0, 0.20912, 0);
  Admitt * a9 = new Admitt(0, 0.55618, 0);
  Admitt * a10 = new Admitt(0, 0.25202, 0);
  Admitt * a11 = new Admitt(0.09498, 0.19890, 0);
  Admitt * a12 = new Admitt(0.12291, 0.25581, 0);
  Admitt * a13 = new Admitt(0.06615, 0.13027, 0);
  Admitt * a14 = new Admitt(0, 0.17615, 0);
  Admitt * a15 = new Admitt(0, 0.11001, 0);
  Admitt * a16 = new Admitt(0.03181, 0.08450, 0);
  Admitt * a17 = new Admitt(0.12711, 0.27038, 0);
  Admitt * a18 = new Admitt(0.08205, 0.19207, 0);
  Admitt * a19 = new Admitt(0.22092, 0.19988, 0);
  Admitt * a20 = new Admitt(0.17093, 0.34802, 0);

  lf->AssocBars(b1, b2, a1);
  lf->AssocBars(b1, b5, a2);
  lf->AssocBars(b2, b3, a3);
  lf->AssocBars(b2, b4, a4);
  lf->AssocBars(b2, b5, a5);
  lf->AssocBars(b3, b4, a6);
  lf->AssocBars(b4, b5, a7);
  lf->AssocBars(b4, b7, a8);
  lf->AssocBars(b4, b9, a9);
  lf->AssocBars(b5, b6, a10);
  lf->AssocBars(b6, b11, a11);
  lf->AssocBars(b6, b12, a12);
  lf->AssocBars(b6, b13, a13);
  lf->AssocBars(b7, b8, a14);
  lf->AssocBars(b7, b9, a15);
  lf->AssocBars(b9, b10, a16);
  lf->AssocBars(b9, b14, a17);
  lf->AssocBars(b10, b11, a18);
  lf->AssocBars(b12, b13, a19);
  lf->AssocBars(b13, b14, a20);

  lf->Execute();

  return 0;
}
