#include "graph.h"
#include <iostream>
#include <string.h>
#include <math.h>

Graph::Graph(int numV):
numV(numV), numE(0), simetric(true)
{
}

Graph::~Graph() {
  bars.clear();
}

void Graph::AddEdge(Bar v, Bar w, Node impd) {
  if (v.GetId() < 0 || v.GetId() >= numV) throw 2;
  if (w.GetId() < 0 || w.GetId() >= numV) throw 2;

  int idV = v.GetId();
  int idW = w.GetId();

  if(idV != idW) {
    this->numE++;
    Assoc(v, w, impd);

    if(simetric /*&& idV != idW*/) {
      Assoc(w, v, impd);
      this->numE++;
    }
  }
}

void Graph::Assoc(Bar v, Bar w, Node impd) {
  int size = bars.size();
  Bar * tmp;
  for(int i = 0; i < size; i++) {
    tmp = &(bars.at(i));
    if(tmp->GetId() == v.GetId()) {
      tmp->AddN(w, impd);
      break;
    }
  }
}

bool Graph::HasEdge(int v, int w) {
  if (v < 0 || v >= numV) throw 2;
  if (w < 0 || w >= numV) throw 2;

  int size = bars.size();
  Bar bV;
  for(int i = 0; i < size; i++) {
    bV = bars.at(i);
    if(bV.GetId() == v) {
      if(bV.HasN(w) != NULL) {
        return true;
      }

      break;
    }
  }

  return false;
}

Node * Graph::GetEdge(int v, int w) {
  if (v < 0 || v >= numV) throw 2;
  if (w < 0 || w >= numV) throw 2;

  int size = bars.size();
  Bar * bV;
  Node * edge;
  for(int i = 0; i < size; i++) {
    bV = &(bars.at(i));
    if(bV->GetId() == v) {
      edge = bV->HasN(w);
      return edge;
    }
  }

  return NULL;
}

void Graph::SetSimetric(bool simetric) {
  this->simetric = simetric;
}

void Graph::AddV(Bar v) {
  int size = bars.size();
  v.SetC(0);
  v.SetS(0);

  if(size < numV) {
    bars.push_back(v);
  }
}

Bar * Graph::at(int v) {
  int size = bars.size();
  Bar * bV;

  if(v < size) {
    for(int i = 0; i < size; i++) {
      bV = &(bars.at(i));
      if(bV->GetId() == v) {
        return bV;
      }
    }
  }

  return NULL;
}

void Graph::AddEdge(Bar v, Bar w, Admitt admitt) {
  double r = admitt.GetR();
  double x = admitt.GetX();
  double sh = (admitt.GetSh() ? admitt.GetSh() : 0);

  double c = r / (pow(r, 2) + pow(x, 2));
  double s = -c;
  Node node(c, s, sh);

  AddEdge(v, w, node);
}
