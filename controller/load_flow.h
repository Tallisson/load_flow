#include <armadillo>
#include <iostream>
#include <vector>
#include "../model/graph.h"

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
  int nPV;
  int nPQ;
  int cont;
  mat jacobian;
  vec estP;
  vec estS;
  vec diffP;
  vec diffS;
  map<int, int> ord;
  map<int, int> ordPQ;

  Graph * bars;
  double error;
  double sBase;
  vector<point> points;
  double max_error;
  bool use_base;

  void mismatches();
  void solveSys();
  void calcJ();
  void initState();
  void initState(double aInitial, double vInitial);
  void initJ();
  void updateState();
  void calcS2();
  void initialize();
  bool nextIterate();

public:
  LoadFlow(double error);
  LoadFlow(double error, double sBase);
  LoadFlow();
  ~LoadFlow();

  int Execute();
  void Execute(double aInitial, double vInitial);
  void AddBar(Bar* bar);
  void AssocBars(Bar* v, Bar* w, Node* node);
  void AssocBars(Bar* v, Bar* w, Admitt* admitt);
  Graph * GetGraph();
  Bar * GetBar(int v);
  Node * GetEdge(int v, int w);
  void SetSimetric(bool s);
  void DpDer();
  void DqDer();
  void SetUseBase(bool use_base);
};
