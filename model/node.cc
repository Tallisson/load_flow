#include "node.h"

double Node::GetC() {
  return c;
}

double Node::GetS() {
  return s;
}

double Node::GetSh() {
  return sh;
}

void Node::SetC(double c) {
  this->c = c;
}

void Node::SetS(double s) {
  this->s = s;
}

void Node::SetSh(double sh) {
  this->sh = sh;
}

void Node::SetPhi(double angle) {
  this->angle_phi = angle;
}

bool Node::Exist() {
  return false;
}

void Node::SetTap(double t) {
  this->tap = t;
}

void Node::SetBar(int bar) {
  this->crt_bar = bar;
}

void Node::SetLim(int var, double value) {
  switch(var) {
  case MAX_TAP:
    this->max_lim_tap = value;
    break;
  case MIN_TAP:
    this->min_lim_tap = value;
    break;
  case MAX_PHI:
    this->max_lim_phi = value;
    break;
  case MIN_PHI:
    this->min_lim_phi = value;
    break;
  default:
    break;
  }
}

double Node::GetTap() {
  return tap;
}

double Node::GetPhi() {
  return angle_phi;
}

int Node::GetType() {
  return type;
}

int Node::GetCrtBar() {
  return crt_bar;
}

double Node::GetLim(int var) {
  switch(var) {
  case MAX_TAP:
    return max_lim_tap;
    break;
  case MIN_TAP:
    return min_lim_tap;
    break;
  case MAX_PHI:
    return max_lim_phi;
    break;
  case MIN_PHI:
    return min_lim_phi;
    break;
  default:
    break;
  }

  return 0;
}
