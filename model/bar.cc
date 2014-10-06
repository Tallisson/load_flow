#include <iostream>
#include "bar.h"
#include <stdio.h>

using namespace std;

namespace load
{

Bar::~Bar() {
  neighbors.clear();
  impd.clear();
}

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

container::map<int, Node*> Bar::GetWeight() {
  return impd;
}

container::map<int, Bar*> Bar::GetNs() {
  return neighbors;
}

double Bar::GetBSh() {
  return bSh;
}

Node* Bar::GetEdge(int n) {
  return impd.at(n);
}

void Bar::SetAPowerG(double aPowerG) {
  this->aPowerG = aPowerG;
}

void Bar::SetAPowerL(double aPowerL) {
  this->aPowerL = aPowerL;
}

void Bar::SetRPowerG(double rPowerG) {
  this->rPowerG = rPowerG;
}

void Bar::SetRPowerL(double rPowerL) {
  this->rPowerL = rPowerL;
}

void Bar::SetEst(int variable, double value) {
  switch(variable) {
  case VOLTAGE:
    this->calcV = value;
    break;
  case A_POWER:
    this->calcP = value;
    break;
  case R_POWER:
    this->calcQ = value;
    break;
  default:
    break;
  }
}

double Bar::GetAPowerG() {
  return this->aPowerG;
}

double Bar::GetAPowerL() {
  return this->aPowerL;
}

double Bar::GetRPowerG() {
  return this->rPowerG;
}

double Bar::GetRPowerL() {
  return this->rPowerL;
}

double Bar::GetEst(int variable) {
  switch(variable) {
  case VOLTAGE:
    return this->calcV;
    break;
  case A_POWER:
    return this->calcP;
    break;
  case R_POWER:
    return this->calcQ;
    break;
  default:
    break;
  }

  return 0;
}

void Bar::SetBSh(double bSh) {
  this->bSh = bSh;
}

void Bar::Clear() {
  neighbors.clear();
  impd.clear();
}
}
