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

double Node::GetTap() {
  return tap;
}

double Node::GetPhi() {
  return angle_phi;
}


int Node::GetType() {
  return type;
}
