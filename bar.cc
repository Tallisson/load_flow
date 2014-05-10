#include <iostream>
#include "bar.h"
#include <stdio.h>

using namespace std;

double Bar::GetAngle() {
  return this->angle;
}

double Bar::GetVoltage() {
  return this->voltage;
}

double Bar::GetAPower() {
  return this->aPower;
}

double Bar::GetRPower() {
  return this->rPower;
}

int Bar::GetType() {
  return this->type;
}

int Bar::GetId() {
  return this->id;
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
