#include "report.h"

namespace load
{

void Report::Insert(Bar* bar, Quantity * q) {
  power.insert(pair<Bar*, Quantity*>(bar, q));
}

void Report::Insert(Node* n, Loss* l) {
  losses.insert(pair<Node*, Loss*>(n, l));
}

Quantity* Report::At(Bar* bar) {
  return power.at(bar);
}

Loss* Report::At(Node* n) {
  return losses.at(n);
}

container::map<Bar*, Quantity*> Report::GetPower() {
  return power;
}

container::map<Node*, Loss*> Report::GetLosses() {
  return losses;
}

Report::Report() {
}

Report::~Report() {
  container::map<Bar*, Quantity*>::iterator itP;
  for(itP = power.begin(); itP != power.end(); itP++)
  {
    delete itP->first;
    delete itP->second;
  }

  container::map<Node*, Loss*>::iterator itL;
  for(itL = losses.begin(); itL != losses.end(); itL++)
  {
    delete itL->first;
    delete itL->second;
  }

  power.clear();
  losses.clear();
}

void Report::Clear()
{
  power.clear();
  losses.clear();
}

}
