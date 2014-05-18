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

    if(barK->GetType() != SLACK) {
      estP(k) = barK->GetVoltage() * barK->GetVoltage() * barK->GetC();
      //cout << barK->GetVoltage() * barK->GetVoltage() * barK->GetC() << endl;
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
      estP(i) = -barK->GetVoltage() * barK->GetVoltage() * (barK->GetS());

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

void LoadFlow::calcS2(int k) {
  double theta;
  Bar * barK = bars->at(k);
  double sum = 0;
  double result = barK->GetVoltage() * barK->GetVoltage() * barK->GetC();

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
  result += barK->GetVoltage() * sum;
  barK->SetAPower(result);

  sum = 0;
  result = -barK->GetVoltage() * barK->GetVoltage() * barK->GetS();

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

  result += barK->GetVoltage() * sum;
  barK->SetRPower(result);
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
  int s = numB << 1;
  for(int k = 0; k < s; k++) {
    for(int m = 0; m < s; m++) {

      if(k < numB && m < numB) {
        calcH(k, m);
      }

      if(k < numB && m >= numB) {
        calcN(k, m);
      }

      if(k >= numB && m < numB) {
        calcM(k, m);
      }

      if(k >= numB && m >= numB) {
        calcL(k, m);
      }
    }
  }
}

void LoadFlow::calcH(int k, int m) {
  double theta;
  Bar * barK = bars->at(k);
  Bar * barM = bars->at(m);

  if(k != m) {
    if(barK->GetType() != SLACK && barM->GetType() != SLACK) {
      Node * edge = bars->GetEdge(k, m);

      if(edge != NULL) {
        theta = barK->GetAngle() - barM->GetAngle();

        jacobian(k, m) = barK->GetVoltage() * barM->GetVoltage() *
                              (edge->GetC() * sin(theta) - edge->GetS() * cos(theta));
      }

      point p(k, m);
      points.push_back(p);
    }
  } else {
    if(barK->GetType() != SLACK) {
        jacobian(k, k) = -barK->GetVoltage() * barK->GetVoltage() * barK->GetS() - estP(k + numB);

        point p(k, k);
        points.push_back(p);
    }
  }

}


void LoadFlow::calcN(int k, int m) {
  double theta;
  int colJ = m - numB;

  Bar * barK = bars->at(k);
  Bar * barM = bars->at(colJ);

  if(k != colJ) {
    if(barK->GetType() != SLACK && barM->GetType() == LOAD) {
      Node * edge = bars->GetEdge(k, colJ);

      if(edge != NULL) {
        theta = barK->GetAngle() - barM->GetAngle();
        jacobian(k, m) = barK->GetVoltage() *
                            (edge->GetC() * cos(theta) + edge->GetS() * sin(theta));
      }

      point p(k, m);
      points.push_back(p);
    }
  } else {
    if(barK->GetType() == LOAD) {
      jacobian(k, m) = (barK->GetVoltage() * barK->GetVoltage() * barK->GetC() + estP(k)) / barK->GetVoltage();

      point p(k, m);
      points.push_back(p);
    }
  }

}

void LoadFlow::calcM(int k, int m) {
  double theta;
  int rowJ = k - numB;
  Bar * barK = bars->at(rowJ);
  Bar *barM = bars->at(m);

  if(rowJ != m) {
    if(barK->GetType() == LOAD && barM->GetType() != SLACK) {
      Node * edge = bars->GetEdge(rowJ, m);

      if(edge != NULL) {
        theta = barK->GetAngle() - barM->GetAngle();
        jacobian(k, m) = -barK->GetVoltage() * barM->GetVoltage() *
                            (edge->GetC() * cos(theta) + edge->GetS() * sin(theta));
      }

      point p(k, m);
      points.push_back(p);
    }
  } else {
    if(barK->GetType() == LOAD) {
      jacobian(k, m) = -barK->GetC() * barK->GetVoltage() * barK->GetVoltage() + estP(rowJ);

      point p(k, m);
      points.push_back(p);
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
          }
          point p(rowJ, colJ);
          points.push_back(p);
        }

      } else {
        if(barK->GetType() == LOAD) {
          jacobian(rowJ, colJ) = (-barK->GetS() * barK->GetVoltage() * barK->GetVoltage() + estP(k + numB)) / barK->GetVoltage();

          point p(rowJ, colJ);
          points.push_back(p);
        }
      }
    }
  }
}

void LoadFlow::calcL(int k, int m) {
  double theta;
  int rowJ = k - numB;
  int colJ = m - numB;

  Bar * barK = bars->at(rowJ);
  Bar * barM = bars->at(colJ);

  if(rowJ != colJ) {
    if(barK->GetType() == LOAD && barM->GetType() == LOAD) {
      Node * edge = bars->GetEdge(rowJ, colJ);

      if(edge != NULL) {
        theta = barK->GetAngle() - barM->GetAngle();
        jacobian(k, m) = barK->GetVoltage() *
                             (edge->GetC() * sin(theta) - edge->GetS() * cos(theta));
      }
      point p(k, m);
      points.push_back(p);
    }

  } else {
    if(barK->GetType() == LOAD) {
      jacobian(k, m) = (-barK->GetS() * barK->GetVoltage() * barK->GetVoltage() + estP(rowJ + numB)) / barK->GetVoltage();

      point p(k, m);
      points.push_back(p);
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
    cout << jacobian << endl;
    diffState();
    updateState();

    counter++;
  }
  //cout << diffP << endl;
  cout << "Número de iterações: " << counter << endl;
  Bar * b;
  for(int i = 0; i < numB; i++) {
    b = bars->at(i);
    if(b->GetType() == SLACK) {
      calcS2(i);
    }

    if(b->GetType() == GENERATION) {
      b->SetRPower(estP(i + numB));
    }
  }

  for(int i = 0; i < numB; i++) {
    b = bars->at(i);

    cout << "Barra(" << b->GetId()+1 << "): Estado(a, v, p, q): (" << b->GetAngle() << ", " << b->GetVoltage() << ", " << b->GetAPower() << ", " << b->GetRPower() << ")" << endl;
    //printf("Angulo: %.4f, Voltagem: %.4f, Potência Ativa: %.4f, Potência Reativa: %.4f\n", b->GetAngle(), b->GetVoltage(), b->GetAPower(), b->GetRPower());
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

  Bar * b;
  for(int i = 0; i < numB; i++) {
    b = bars->at(i);
    if(b->GetType() == SLACK) {
      calcS2(i);
    }

    if(b->GetType() == GENERATION) {
      b->SetRPower(estP(i + numB));
    }
  }
  cout << "Número de iterações: " << counter << endl;
  for(int i = 0; i < numB; i++) {
    b = bars->at(i);
    cout << "Barra(" << b->GetId() << "): Estado(a, v, p, q): (" << b->GetAngle() << ", " << b->GetVoltage() << ", " << b->GetAPower() << ", " << b->GetRPower() << ")" << endl;
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

void LoadFlow::SetSimetric(bool s) {
  bars->SetSimetric(s);
}

void LoadFlow::AssocBars(Bar v, Bar w, Admitt admitt) {
  bars->AddEdge(v, w, admitt);
}
