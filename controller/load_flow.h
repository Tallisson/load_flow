#include <armadillo>
#include <iostream>
#include <stdio.h>
#include "../model/graph.h"

using namespace arma;
using namespace std;

class LoadFlow {
private:
  int numB;
  int nPV;
  int nPQ;
  int cont;

  int nLT;
  int nTAP_Fixed;
  int nTap_VC;
  int nTap_MVAR;
  int nTAP_PHASE;

  mat jacobian;

  vec estP;
  vec estS;
  vec diffP;
  vec diffS;
  vec loss;

  map<int, int> ord;
  map<int, int> ordPQ;
  map<Node*, double> estCrtlVar;

  Graph * bars;

  double error;
  double sBase;

  double max_error;
  bool use_base;
  bool verbose;
  double s_alpha;

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
  void setControlVariables();

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
  void LossCalc();
};
