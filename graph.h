#include <vector>
#include "bar.h"

using namespace std;

class Graph {
private:
  int numV;
  int numE;
  Node * edge;
  bool simetric;
  void Assoc(Bar v, Bar w, Node impd);
  vector<Bar> bars;

public:
  Graph(int numV);
  ~Graph();

  void AddV(Bar v);
  void AddEdge(Bar v, Bar w, Node impd);
  bool HasEdge(int v, int w);
  Node * GetEdge(int v, int w);
  void SetSimetric(bool simetric);
  Bar * at(int v);
};
