#include <armadillo>
#include <iostream>
#include <cmath>

#define SLACK_BAR 1
#define LOAD_BAR 2
#define GENERATION_BAR 3

#define ANGLE_INIT 0
#define VOLTAGE_INIT 1

typedef struct point {
 int row;
 int col;

 point(int row, int col): row(row), col(col) {
 };
} point;

using namespace arma;
using namespace std;

// Protótipos
void calcPower();
void diffPower();
void diffState();
void initStates();
void updateState();
void execute();

bool nextIterate();

void calcH();
void calcL();
void calcM();
void calcN();
void calcJ();

int numBars = 5;
int b2 = numBars << 1;
double error = 0.0001;

// Tipos de Barras
vec bar = "1 3 2 2 2";

vector<point> points;

// Variáveis descrevendo as barras
vec aPower =  "0 0.2 -0.45 -0.4 -0.6";//"0 -0.40";
vec angle = "0 0 0 0 0 0";
vec voltage = "1.06 1.02 0 0 0";
vec rPower = "0 0 -0.15 -0.05 -0.1";

// Matrizes de condutância e susceptância
mat conductance = "0 5 1.25 0 0; 5 0 1.666 0 2.5; 0 0 0 10 0; 0 0 0 0 1.25; 0 2.5 0 1.25 0";//"0.1923 -0.1923; -0.1923 0.1923"; //"3.8462 -3.8462; -3.8462 3.8462";
mat susceptance = "0 -15 -3.75 0 0; 0 0 -5 -5 -7.5; 0 0 0 -30 0; 0 0 0 0 -3.75; 0 -7.5 0 -3.75 0"; //"-19.2308 19.2308; 19.2308 -19.2308";

// Matriz jacobiana
mat jacobian = zeros<mat>(b2, b2);

// Vetor de potências estimado
vec estPower = zeros<vec>(b2);
vec estState = zeros<vec>(b2);

// Matriz variações de estado e potência
vec dState = zeros<vec>(b2);
vec dPower = zeros<vec>(b2);

void calcPower() {
  double theta;
  double sum;
  int i;
  int k, m;

  for(k = 0; k < numBars; k++) {
    sum = 0;

    if(bar(k) != SLACK_BAR) {
      estPower(k) = (voltage(k)*voltage(k)) * conductance(k, k);

      for(m = 0; m < numBars; m++) {
        if(k != m) {
          if(conductance(k, m) != 0 || susceptance(k, m) != 0) {
            theta =  angle(k) - angle(m);

            sum += voltage(m) * (conductance(k, m) * cos(theta) +
                              susceptance(k, m) * sin(theta));
          }
        }
      }

      estPower(k) += voltage(k) * sum;
    }

    i = k + numBars;
    sum = 0;

    if(bar(k) == LOAD_BAR) {
      estPower(i) = -(voltage(k)*voltage(k)) * susceptance(k, k);

      for(m = 0; m < numBars; m++) {
        if(k != m) {
          if(conductance(k, m) != 0 || susceptance(k, m) != 0) {
            theta = angle(k) - angle(m);

            sum += voltage(m) * (conductance(k, m) * sin(theta) -
                              susceptance(k, m) * cos(theta));
          }
        }
      }

      estPower(i) += voltage(k) * sum;
    }

  }
}

void diffPower() {
  int i = 0;

  for(int k = 0; k < numBars; k++) {
    if(bar(k) != SLACK_BAR) {
      dPower(k) = aPower(k) - estPower(k);
    }

    if(bar(k) == LOAD_BAR) {
      i = k + numBars;
      dPower(i) = rPower(k) - estPower(i);
    }
  }
}

void diffState() {
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
  dState = jacobian * dPower;
}

void calcH() {
  double theta;
  //double sum;

  for(int k = 0; k < numBars; k++) {
    for(int m = 0; m < numBars; m++) {
      if(k != m) {
        if(bar(k) != SLACK_BAR && bar(m) != SLACK_BAR) {

          if(conductance(k, m) != 0 || susceptance(k, m) != 0) {
            theta = angle(k) - angle(m);

            jacobian(k, m) = voltage(k) * voltage(m) *
                                  (conductance(k, m) * sin(theta) - susceptance(k, m) * cos(theta));

            point p(k, m);
            points.push_back(p);
          }
        }
      } else {
        if(bar(k) != SLACK_BAR && bar(m) != SLACK_BAR) {

          jacobian(k, k) = -(voltage(k)*voltage(k)) * susceptance(k, k) - estPower(k + numBars);

          point p(k, m);
          points.push_back(p);

          /*sum = 0;
          for(int t = 0; t < numBars; t++) {
            if(k != t) {

              if(conductance(k, t) != 0 || susceptance(k, t) != 0) {
                theta = angle(t) - angle(k);

                sum += voltage(t) * (conductance(k, t) * sin(theta) -
                                      susceptance(k, t) * cos(theta));
              }
            }
          }

          jacobian(k, k) -= voltage(k) * sum;*/
        }
      }
    }
  }
}

void calcN() {
  double theta;
  int colJ;

  for(int k = 0; k < numBars; k++) {
    for(int m = 0; m < numBars; m++) {

      colJ = m + numBars;
      if(k != m) {
        if(bar(k) != SLACK_BAR && bar(m) == LOAD_BAR) {

          if(conductance(k, m) != 0 || susceptance(k, m) != 0) {
            theta = angle(k) - angle(m);

            jacobian(k, colJ) = voltage(k) *
                                (conductance(k, m) * cos(theta) + susceptance(k, m) * sin(theta));

            point p(k, colJ);
            points.push_back(p);
          }
        }
      } else {
        if(bar(k) != SLACK_BAR && bar(m) == LOAD_BAR) {

          jacobian(k, colJ) = 1/voltage(k) * (voltage(k) * voltage(k) * conductance(k, k) + estPower(k));

          point p(k, colJ);
          points.push_back(p);
          /*for(int t = 0; t < numBars; t++) {
            if(k != t) {

              if(conductance(k, t) != 0 || susceptance(k, t) != 0) {
                theta = angle(t) - angle(k);

                jacobian(k, colJ) += voltage(t) *
                                    (conductance(k, t) * cos(theta) + susceptance(k, t) * sin(theta));
              }
            }
          }*/
        }
      }
    }
  }

}

void calcM() {
  double theta;
  int rowJ;

  for(int k = 0; k <  numBars; k++) {
    for(int m = 0; m < numBars; m++) {

      rowJ = k + numBars;
      if(k != m) {
        if(bar(k) == LOAD_BAR && bar(m) != SLACK_BAR) {

          if(conductance(k, m) != 0 || susceptance(k, m) != 0) {
            theta = angle(k) - angle(m);

            jacobian(rowJ, m) = -voltage(k) * voltage(m) *
                                (conductance(k, m) * cos(theta) + susceptance(k, m) * sin(theta));

            point p(rowJ, m);
            points.push_back(p);
          }
        }
      } else {
        if(bar(k) == LOAD_BAR && bar(m) != SLACK_BAR) {

          jacobian(rowJ, m) = -(voltage(k)*voltage(k)) * conductance(k, k) + estPower(k);

          point p(rowJ, m);
          points.push_back(p);
          /*for(int t = 0; t < numBars; t++) {
            if(k != t) {

              if(conductance(k, t) != 0 || susceptance(k, t) != 0) {
                theta = angle(t) - angle(k);

                sum += voltage(t) *
                               (conductance(k, t) * cos(theta) + susceptance(k, t) * sin(theta));
              }
            }
          }

          jacobian(k, k) += voltage(k) * sum;*/
        }
      }
    }
  }
}

void calcL() {
  double theta;
  int rowJ, colJ;

  for(int k = 0; k < numBars; k++) {
    for(int m = 0; m < numBars; m++) {

      rowJ = k + numBars;
      colJ = m + numBars;
      if(k != m) {
        if(bar(k) == LOAD_BAR && bar(m) == LOAD_BAR) {

          if(conductance(k, m) != 0 || susceptance(k, m) != 0) {
            theta = angle(k) - angle(m);

            jacobian(rowJ, colJ) = voltage(k) *
                                 (conductance(k, m) * sin(theta) - susceptance(k, m) * cos(theta));

            point p(rowJ, colJ);
            points.push_back(p);
          }
        }

      } else {
        if(bar(k) == LOAD_BAR && bar(m) == LOAD_BAR) {

          jacobian(rowJ, colJ) = 1/voltage(k) * (estPower(rowJ)-(voltage(k)*voltage(k)) * susceptance(k, k));

          point p(rowJ, colJ);
          points.push_back(p);
          /*for(int t = 0; t < numBars; t++) {
            if(k != t) {

              if(conductance(k, t) != 0 || susceptance(k, t) != 0) {
                theta = angle(t) - angle(k);

                jacobian(k, k) += voltage(t) *
                                    (conductance(k, t) * sin(theta) + susceptance(k, t) * cos(theta));
              }
            }
          }*/
        }
      }
    }
  }
}

void calcJ() {
  calcH();
  calcN();
  calcM();
  calcL();
}

void initState() {
  for(int k = 0; k < b2; k++) {
    if(k < numBars) {

      if(bar(k) != SLACK_BAR) {
        angle(k) = ANGLE_INIT;
      }
    } else {
      int i = k - numBars;

      if(bar(i) == LOAD_BAR) {
        voltage(i) = VOLTAGE_INIT;
      }

    }
  }
}

void updateState() {
  for(int k = 0; k < b2; k++) {
    if(k < numBars) {

      if(bar(k) != SLACK_BAR)
        angle(k) += dState(k);
    } else {
      int i = k - numBars;

      if(bar(i) == LOAD_BAR)
        voltage(i) += dState(k);
    }
  }
}

bool nextIterate() {
  int i = 0;

  for(int k = 0; k < numBars; k++) {
    if( error < abs(dPower(k)) ) {
      return true;
    }

    i = k + numBars;
    if( error < abs(dPower(i)) ) {
      return true;
    }
  }

  return false;
}

void execute() {
  int counter = 0;

  initState();
  //while(true) {
    calcPower();
    diffPower();

    /*if(nextIterate() == false)
      break;*/

    calcJ();
    cout << jacobian << endl;
    diffState();
    updateState();

    counter++;
  //}

  cout << "Num de iterações = " << counter << endl;

  cout << "Angulos" << endl;
  cout << angle << endl;

  cout << "Voltagem" << endl;
  cout << voltage;
}

/*int main(int argc, char ** argv) {
  execute();

  return 0;
}*/
