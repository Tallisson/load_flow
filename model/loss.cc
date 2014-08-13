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
  case P_OUT:
    this->p_out_branch = value;
  case Q_IN:
    this->q_in_branch= value;
    break;
  case Q_OUT:
    this->q_out_branch = value;
    break;
  case LOSS:
    this->loss = value;
    break;
  }
}

double Loss::GetAttr(int attr) {
  switch(attr) {
  case P_IN:
    return this->p_in_branch;
    break;
  case P_OUT:
    return this->p_out_branch;
    break;
  case Q_IN:
    return this->q_in_branch;
    break;
  case Q_OUT:
    return this->q_out_branch;
    break;
  case LOSS:
    return this->loss;
    break;
  }

  return 0;
}
