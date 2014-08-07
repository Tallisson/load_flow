#include "admitt.h"

void Admitt::SetR(double r) {
  this->r = r;
}

void Admitt::SetX(double x) {
  this->r = x;
}

void Admitt::SetSh(double sh) {
  this->sh = sh;
}

void Admitt::SetTap(double tap) {
  this->tap  = tap;
}

void Admitt::SetType(int type) {
  this->type = type;
}

double Admitt::GetR() {
  return r;
}

double Admitt::GetX() {
  return x;
}

double Admitt::GetSh() {
  return sh;
}

int Admitt::GetType() {
  return type;
}
