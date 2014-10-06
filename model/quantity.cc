#include "quantity.h"

namespace load
{

Quantity::Quantity(): pg(0), pc(0), qg(0), qc(0) {
}

Quantity::~Quantity() {
}

void Quantity::SetAttr(int attr, double value) {
  switch(attr) {
  case PG:
    this->pg = value;
    break;
  case PC:
    this->pc= value;
    break;
  case QG:
    this->qg= value;
    break;
  case QC:
    this->qc= value;
    break;
  }
}

double Quantity::GetAttr(int attr) {
  switch(attr) {
  case PG:
    return this->pg;
    break;
  case PC:
    return this->pc;
    break;
  case QG:
    return this->qg;
    break;
  case QC:
    return this->qc;
    break;
  }

  return 0;
}
}
