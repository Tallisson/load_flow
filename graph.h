#include <map>
#include "bar.h"
#include "admitt.h"

using namespace std;

class Graph {
private:
  int numV;
  int numE;
  bool simetric;
  void Assoc(Bar * v, Bar * w, Node * impd);
  map<int, Bar*> bars;

public:
  Graph(int numV);
  Graph();
  ~Graph();

  void AddV(Bar * v);
  void AddEdge(Bar * v, Bar * w, Node * impd);
  bool HasEdge(int v, int w);
  Node * GetEdge(int v, int w);
  void SetSimetric(bool simetric);
  Bar * at(int v);
  void AddEdge(Bar * v, Bar * w, Admitt * admitt);
  map<int, Bar*> GetBars();
  int GetSize();
};
