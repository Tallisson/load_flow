//#include <map>
#include <boost/container/map.hpp>
#include "bar.h"
#include "admitt.h"

//using namespace std;
using namespace boost;

class Graph {
private:
  int numV;
  int numE;
  bool simetric;
  void Assoc(Bar * v, Bar * w, Node * impd);
  container::map<int, Bar*> bars;

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
  Node * AddEdge(Bar * v, Bar * w, Admitt * admitt);
  container::map<int, Bar*> GetBars();
  int GetSize();
};
