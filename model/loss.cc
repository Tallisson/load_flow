#include "loss.h"

Loss::Loss() {
}

Loss::~Loss() {
}

void Loss::SetAttr(int attr, double value) {
  switch(attr) {
  case P_IN:
    this->p_in_branch = value;
    break;
  case Q_IN:
    this->q_in_branch= value;
    break;
  }
}

double Loss::GetAttr(int attr) {
  switch(attr) {
  case P_IN:
    return this->p_in_branch;
    break;
  case Q_IN:
    return this->q_in_branch;
    break;
  }

  return 0;
}
