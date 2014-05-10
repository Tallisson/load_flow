#include "load_flow.h"
#include <stdio.h>

using namespace std;

#define ANGLE_INIT 0
#define VOLTAGE_INIT 1

LoadFlow::LoadFlow(int numB, double error):
    numB(numB), error(error) {
  bars = new Graph(numB);

  int size = numB << 1;
  jacobian = zeros<mat>(size, size);
  estP = zeros<vec>(size);
  estS = zeros<vec>(size);
  diffP = zeros<vec>(size);
  diffS = zeros<vec>(size);
}

LoadFlow::~LoadFlow() {
  delete bars;
}

void LoadFlow::AddBar(Bar bar) {
  bars->AddV(bar);
}

Graph * LoadFlow::GetGraph() {
  return this->bars;
}

void LoadFlow::initState() {
  Bar * bar;

  for(int k = 0; k < numB; k++) {
    if(k < numB) {
      bar = bars->at(k);

      if(bar->GetType() != SLACK) {
        bar->SetAngle(ANGLE_INIT);
      }

      if(bar->GetType() == LOAD) {
        bar->SetVoltage(VOLTAGE_INIT);
      }
    }
  }
}

void LoadFlow::initState(double aInitial, double vInitial) {
  Bar * bar;

  for(int k = 0; k < numB; k++) {
    if(k < numB) {
      bar = bars->at(k);

      if(bar->GetType() != SLACK) {
        bar->SetAngle(aInitial);
      }

      if(bar->GetType() == LOAD) {
        bar->SetVoltage(vInitial);
      }
    }
  }
}

void LoadFlow::updateState() {
  Bar * barK;

  for(int k = 0; k < numB; k++) {
    barK = bars->at(k);

    if(barK->GetType() != SLACK)
      barK->SetAngle(barK->GetAngle() + diffS(k));

    if(barK->GetType() == LOAD) {
      int i = k + numB;
      barK->SetVoltage(barK->GetVoltage() + diffS(i));
    }
  }
}

bool LoadFlow::nextIterate() {
  int i = 0;

  for(int k = 0; k < numB; k++) {
    if( error < abs(diffP(k)) ) {
      return true;
    }

    i = k + numB;
    if( error < abs(diffP(i)) ) {
      return true;
    }
  }

  return false;
}

void LoadFlow::calcPower() {
  double theta;
  double sum;

  for(int k = 0; k < numB; k++) {
    sum = 0;
    Bar * barK = bars->at(k);
    Node * edgeK;

    if(barK->GetType() != SLACK) {
      edgeK = bars->GetEdge(k, k);

      estP(k) = (barK->GetVoltage()*barK->GetVoltage());
      if(edgeK != NULL)
        estP(k) *= edgeK->GetC();

      for(int m = 0; m < numB; m++) {
        if(k != m) {
          Bar * barM = bars->at(m);
          Node * edge = bars->GetEdge(k, m);

          if(edge != NULL) {
            theta =  barK->GetAngle() - barM->GetAngle();

            sum += barM->GetVoltage() * (edge->GetC() * cos(theta) +
                              edge->GetS() * sin(theta));
          }
        }
      }

      estP(k) += (barK->GetVoltage() * sum);
    }

    int i = k + numB;
    sum = 0;

    if(barK->GetType() != SLACK) {
      estP(i) = -(barK->GetVoltage()*barK->GetVoltage());

      if(edgeK != NULL)
        estP(i) *= edgeK->GetS();

      for(int m = 0; m < numB; m++) {
        if(k != m) {
          Bar * barM = bars->at(m);
          Node * edge = bars->GetEdge(k, m);

          if(edge != NULL) {
            theta =  barK->GetAngle() - barM->GetAngle();

            sum += barM->GetVoltage() * (edge->GetC() * sin(theta) -
                              edge->GetS() * cos(theta));
          }
        }
      }

      estP(i) += barK->GetVoltage() * sum;
    }

  }
}

void LoadFlow::diffPower() {
  int i = 0;
  Bar * barK;

  for(int k = 0; k < numB; k++) {
    barK = bars->at(k);

    if(barK->GetType() != SLACK) {
      diffP(k) = barK->GetAPower() - estP(k);
    }

    if(barK->GetType() == LOAD) {
      i = k + numB;
      diffP(i) = barK->GetRPower() - estP(i);
    }
  }
}

void LoadFlow::diffState() {
  vector<point>::size_type s = sqrt(points.size());
  mat m(s, s);

  int k = 0;
  for(unsigned i = 0; i < s; i++) {
    for(unsigned j = 0; j < s; j++) {
      point p = points[k++];
      m(i, j) = jacobian(p.row, p.col);
    }
  }

  m = inv(m);
  k = 0;
  for(unsigned i = 0; i < s; i++) {
    for(unsigned j = 0; j < s; j++) {
      point p = points[k++];
      jacobian(p.row, p.col) = m(i, j);
    }
  }

  points.clear();
  diffS = jacobian * diffP;
}

void LoadFlow::calcJ() {
  calcH();
  calcN();
  calcM();
  calcL();
}

void LoadFlow::calcH() {
  double theta;

  for(int k = 0; k < numB; k++) {
    Bar * barK;

    for(int m = 0; m < numB; m++) {
      barK = bars->at(k);
      Bar * barM = bars->at(m);

      if(k != m) {
        if(barK->GetType() != SLACK && barM->GetType() != SLACK) {
          Node * edge = bars->GetEdge(k, m);

          if(edge != NULL) {

            theta = barK->GetAngle() - barM->GetAngle();

            jacobian(k, m) = barK->GetVoltage() * barM->GetVoltage() *
                                  (edge->GetC() * sin(theta) - edge->GetS() * cos(theta));

            point p(k, m);
            points.push_back(p);
          }
        }
      } else {
        if(barK->GetType() != SLACK) {
          Node * edge = bars->GetEdge(k, k);
          if(edge != NULL)
            jacobian(k, k) = -(barK->GetVoltage()*barK->GetVoltage()) * edge->GetS();

          jacobian(k, k) -= estP(k + numB);

          point p(k, m);
          points.push_back(p);

        }
      }
    }
  }
}

void LoadFlow::calcN() {
  double theta;
  int colJ;

  for(int k = 0; k < numB; k++) {
    Bar * barK;

    for(int m = 0; m < numB; m++) {
      colJ = m + numB;
      barK = bars->at(k);
      Bar * barM = bars->at(m);

      if(k != m) {
        if(barK->GetType() != SLACK && barM->GetType() == LOAD) {
          Node * edge = bars->GetEdge(k, m);

          if(edge != NULL) {
            theta = barK->GetAngle() - barM->GetAngle();

            jacobian(k, colJ) = barK->GetVoltage() *
                                (edge->GetC() * cos(theta) + edge->GetS() * sin(theta));

            point p(k, colJ);
            points.push_back(p);
          }
        }
      } else {
        if(barK->GetType() == LOAD) {
          Node * edge = bars->GetEdge(k, k);

          if(edge != NULL) {
            jacobian(k, colJ) = barK->GetVoltage() * barK->GetVoltage() * edge->GetC();
          }
          jacobian(k, colJ) = (jacobian(k, colJ) + estP(k)) / barK->GetVoltage();

          point p(k, colJ);
          points.push_back(p);
        }
      }
    }
  }

}

void LoadFlow::calcM() {
  double theta;
  int rowJ;

  for(int k = 0; k < numB; k++) {
    Bar * barK = bars->at(k);

    for(int m = 0; m < numB; m++) {
      rowJ = k + numB;
      Bar * barM = bars->at(m);

      if(k != m) {
        if(barK->GetType() == LOAD && barM->GetType() != SLACK) {
          Node * edge = bars->GetEdge(k, m);

          if(edge != NULL) {
            theta = barK->GetAngle() - barM->GetAngle();

            jacobian(rowJ, m) = -barK->GetVoltage() * barM->GetVoltage() *
                                (edge->GetC() * cos(theta) + edge->GetS() * sin(theta));

            point p(rowJ, m);
            points.push_back(p);
          }
        }
      } else {
        if(barK->GetType() == LOAD) {
          Node * edge = bars->GetEdge(k, m);

          if(edge != NULL) {
            jacobian(rowJ, k) = -edge->GetC() * (barK->GetVoltage()*barK->GetVoltage()) ;
          }
          jacobian(rowJ, k) += estP(k);

          point p(rowJ, k);
          points.push_back(p);
        }
      }
    }
  }
}

void LoadFlow::calcL() {
  double theta;
  int rowJ, colJ;

  for(int k = 0; k < numB; k++) {
    Bar * barK = bars->at(k);

    for(int m = 0; m < numB; m++) {
      rowJ = k + numB;
      colJ = m + numB;
      Bar * barM = bars->at(m);

      if(k != m) {
        if(barK->GetType() == LOAD && barM->GetType() == LOAD) {
          Node * edge = bars->GetEdge(k, m);

          if(edge != NULL) {
            theta = barK->GetAngle() - barM->GetAngle();

            jacobian(rowJ, colJ) = barK->GetVoltage() *
                                 (edge->GetC() * sin(theta) - edge->GetS() * cos(theta));

            point p(rowJ, colJ);
            points.push_back(p);
          }
        }

      } else {
        if(barK->GetType() == LOAD) {
          Node * edge = bars->GetEdge(k, m);

          if(edge != NULL) {
            jacobian(rowJ, colJ) = -edge->GetS() * (barK->GetVoltage() * barK->GetVoltage());
          }
          jacobian(rowJ, colJ) = (jacobian(rowJ, colJ) + estP(k + numB)) / barK->GetVoltage();

          point p(rowJ, colJ);
          points.push_back(p);
        }
      }
    }
  }
}
void LoadFlow::Execute() {
  int counter = 0;

  initState();
  while(true) {
    calcPower();
    diffPower();

    if(nextIterate() == false)
      break;

    calcJ();
    diffState();
    updateState();

    counter++;
  }

  cout << "Número de iterações: " << counter << endl;
  Bar * b;
  for(int i = 0; i < numB; i++) {
    b = bars->at(i);

    cout << "Barra(" << b->GetId() << "): Estado(a, v): (" << b->GetAngle() << ", " << b->GetVoltage() << ")" << endl;
  }
}

void LoadFlow::Execute(double aInitial, double vInitial) {
  int counter = 0;

  initState(aInitial, vInitial);
  while(counter < 2) {
    calcPower();
    diffPower();

    if(nextIterate() == false)
      break;

    calcJ();
    diffState();
    updateState();

    counter++;
  }

  cout << "Número de iterações: " << counter << endl;
  Bar * b;
  for(int i = 0; i < numB; i++) {
    b = bars->at(i);

    cout << "Barra(" << b->GetId() << "): Estado(a, v): (" << b->GetAngle() << ", " << b->GetVoltage() << ")" << endl;
  }
}

Bar * LoadFlow::GetBar(int v) {
  return bars->at(v);
}

void LoadFlow::AssocBars(Bar v, Bar w, Node node) {
  bars->AddEdge(v, w, node);
}

Node * LoadFlow::GetEdge(int v, int w) {
  Node * edge = bars->GetEdge(v, w);
  return edge;
}
