#include "admitt.h"

namespace load
{

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

void Admitt::SetAngle(double angle) {
  this->angle_phi = angle;
}

void Admitt::SetMaxLim(double max) {
  max_lim = max;
}

void Admitt::SetMinLim(double min) {
  min_lim = min;
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

double Admitt::GetAngle() {
  return angle_phi;
}

double Admitt::GetTap() {
  return tap;
}

int Admitt::GetTo() {
  return to;
}

int Admitt::GetFrom() {
  return from;
}

double Admitt::GetMaxLim() {
  return max_lim;
}

double Admitt::GetMinLim() {
  return min_lim;
}

int Admitt::GetCrtBar() {
  return crt_bar;
}
}
