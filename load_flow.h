#include <armadillo>
#include <iostream>
#include <vector>
#include "graph.h"

using namespace arma;
using namespace std;

typedef struct point {
 int row;
 int col;

 point(int row, int col): row(row), col(col) {
 };
} point;

class LoadFlow {
private:
  int numB;
  mat jacobian;
  vec estP;
  vec estS;
  vec diffS;
  vec diffP;

  Graph * bars;
  double error;
  vector<point> points;

  void calcPower();
  void diffPower();
  void diffState();
  void calcH();
  void calcN();
  void calcM();
  void calcL();
  void calcH(int k, int m);
  void calcN(int k, int m);
  void calcM(int k, int m);
  void calcL(int k, int m);
  void calcJ();
  void initState();
  void initState(double aInitial, double vInitial);
  void updateState();
  void calcS2(int k);

  bool nextIterate();

public:
  LoadFlow(int numB, double error);
  ~LoadFlow();

  void Execute();
  void Execute(double aInitial, double vInitial);
  void AddBar(Bar bar);
  void AssocBars(Bar v, Bar w, Node node);
  void AssocBars(Bar v, Bar w, Admitt admitt);
  Graph * GetGraph();
  Bar * GetBar(int v);
  Node * GetEdge(int v, int w);
  void SetSimetric(bool s);
};
