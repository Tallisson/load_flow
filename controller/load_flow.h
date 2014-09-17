#ifndef LOAD_FLOW_H_
#define LOAD_FLOW_H_

#include <armadillo>
#include <iostream>
#include <stdio.h>
#include <boost/algorithm/string.hpp>
#include "../model/graph.h"
#include "../model/report.h"
#include "../model/define.h"
#include "../model/utils.h"

using namespace arma;
using namespace std;
using namespace boost;

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

  container::map<int, int> ord;
  container::map<int, int> ordPQ;
  container::map<Node*, double> estCrtlVar;

  Graph * bars;

  double error;
  double sBase;

  double max_error;
  bool use_base;
  bool verbose;
  double s_alpha;

  Report * report;
  Desc* description;

  void mismatches();
  void solveSys();
  void calcJ();
  void initState();
  void initState(double aInitial, double vInitial);
  void initJ();
  void updateState();
  void calcS2();
  void initialize(const char* file);
  void initialize();
  bool nextIterate();
  void setControlVariables();
  //void insertLoss(Node* edge, double vK, double vM, double theta);
  void insertLoss(Node* edge, Bar* barK, Bar* barM);
public:
  LoadFlow(double error);
  LoadFlow(double error, double sBase);
  LoadFlow();
  ~LoadFlow();

  int Execute();
  int Execute(const char* file);
  void Execute(double aInitial, double vInitial);
  void AddBar(Bar* bar);
  void AssocBars(Bar* v, Bar* w, Node* node);
  void AssocBars(Bar* v, Bar* w, Admitt* admitt);
  void AssocBars(Admitt* admitt);
  Graph * GetGraph();
  Bar * GetBar(int v);
  Node * GetEdge(int v, int w);
  void SetSimetric(bool s);
  void DpDer();
  void DqDer();
  void SetUseBase(bool use_base);
  void CalcReport();
};

#endif /* LOAD_FLOW_H_ */
