#include <iostream>
#include "bar.h"
#include <stdio.h>

using namespace std;

double Bar::GetAngle() {
  return angle;
}

double Bar::GetVoltage() {
  return voltage;
}

double Bar::GetAPower() {
  return aPower;
}

double Bar::GetRPower() {
  return rPower;
}

int Bar::GetType() {
  return type;
}

int Bar::GetId() {
  return id;
}

void Bar::SetAngle(double angle) {
  this->angle = angle;
}

void Bar::SetVoltage(double voltage) {
  this->voltage = voltage;
}

void Bar::SetAPower(double aPower) {
  this->aPower = aPower;
}

void Bar::SetRPower(double rPower) {
  this->rPower = rPower;
}

void Bar::AddN(Bar* n, Node* i) {
  c += i->GetC();
  s += i->GetS() + i->GetSh();

  int id = n->GetId();
  neighbors.insert(pair<int, Bar*>(id, n));
  impd.insert(pair<int, Node*>(id, i));
}

Node * Bar::HasN(int w) {
  Bar* bW = neighbors.at(w);
  if(bW != NULL) {
    Node* imp = impd.at(w);
    return imp;
  }

  return NULL;
}

double Bar::GetC() {
  return c;
}

double Bar::GetS() {
  return s;
}

void Bar::SetS(double s) {
  this->s = s;
}

void Bar::SetC(double c) {
  this->c = c;
}

map<int, Node*> Bar::GetWeight() {
  return impd;
}

map<int, Bar*> Bar::GetNs() {
  return neighbors;
}

int Bar::GetBSh() {
  return bSh;
}

Node* Bar::GetEdge(int n) {
  return impd.at(n);
}
