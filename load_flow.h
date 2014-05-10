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
  void calcJ();
  void initState();
  void initState(double aInitial, double vInitial);
  void updateState();

  bool nextIterate();

public:
  LoadFlow(int numB, double error);
  ~LoadFlow();

  void Execute();
  void Execute(double aInitial, double vInitial);
  void AddBar(Bar bar);
  void AssocBars(Bar v, Bar w, Node node);
  Graph * GetGraph();
  Bar * GetBar(int v);
  Node * GetEdge(int v, int w);
};
