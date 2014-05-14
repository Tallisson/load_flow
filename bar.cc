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

void Bar::AddN(Bar n, Node i) {
  c += -i.GetC();
  s += -i.GetS() + i.GetSh();
  neighbors.push_back(n);
  impd.push_back(i);
}

Node * Bar::HasN(int w) {
  int size = neighbors.size();
  Bar bW;
  for(int i = 0; i < size; i++) {
    bW = neighbors.at(i);
    if(bW.GetId() == w) {
      Node * imp = &(impd.at(i));
      return imp;
    }
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
