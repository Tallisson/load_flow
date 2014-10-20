#include "graph.h"
#include <iostream>
#include <string.h>
#include <math.h>

namespace load
{
Graph::Graph(int numV):
numV(numV), numE(0), simetric(true)
{
}

Graph::Graph():
numV(0), numE(0), simetric(true)
{
}

Graph::~Graph() {
  container::map<int, Bar*>::iterator it;
  for(it = bars.begin(); it != bars.end(); it++)
  {
    delete it->second;
    it->second = NULL;
  }

  bars.clear();
}

void Graph::AddEdge(Bar* v, Bar* w, Node* impd) {
  int idV = v->GetId();
  int idW = w->GetId();

  if(idV != idW) {
    this->numE++;
    Assoc(v, w, impd);

    if(simetric) {
      Assoc(w, v, impd);
      this->numE++;
    }
  }
}

void Graph::Assoc(Bar* v, Bar* w, Node* impd) {
  int idV = v->GetId();
  Bar * tmp = bars.at(idV);

  if(tmp != NULL) {
    tmp->AddN(w, impd);
  }
}

bool Graph::HasEdge(int v, int w) {
  Bar * bV = bars.at(v);
  if(bV != NULL) {
    if(bV->HasN(w) != NULL) {
      return true;
    }
  }

  return false;
}

Node * Graph::GetEdge(int v, int w) {
  Bar * bV;
  Node * edge;

  bV = bars.at(v);
  if(bV != NULL) {
    edge = bV->HasN(w);
    return edge;
  }

  return NULL;
}

void Graph::SetSimetric(bool simetric) {
  this->simetric = simetric;
}

void Graph::AddV(Bar* v) {
  int idV = v->GetId();
  bars.insert(pair<int, Bar*>(idV, v));
  numV++;
}

Bar * Graph::at(int v) {
  Bar * bV = bars.at(v);

  return bV;
}

Node * Graph::AddEdge(Bar* v, Bar* w, Admitt* admitt) {
  double r = admitt->GetR();
  double x = admitt->GetX();
  double sh = (admitt->GetSh() ? admitt->GetSh() : 0);

  double c = r / (pow(r, 2) + pow(x, 2));
  double s = -x / (pow(r, 2) + pow(x, 2));

  Node * node = new Node(c, s, sh, admitt->GetTap(), admitt->GetAngle(), admitt->GetType(),
                         admitt->GetMinLim(), admitt->GetMaxLim(),
                         admitt->GetFrom(), admitt->GetTo(), admitt->GetCrtBar());

  AddEdge(v, w, node);

  return node;
}

container::map<int, Bar*> Graph::GetBars() {
  return bars;
}

int Graph::GetSize() {
  return bars.size();
}
}
