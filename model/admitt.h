#ifndef ADMITT_H_
#define ADMITT_H_

#include "define.h"
#include <boost/container/vector.hpp>
#include <boost/cstdlib.hpp>

using namespace std;
using namespace boost;

class Admitt {
private:
  double r;
  double x;
  double sh;
  double tap;
  double angle_phi;
  int type;
  int to;
  int from;

public:
  Admitt(double r, double x, double sh): r(r), x(x), sh(sh), tap(1), angle_phi(0), type(TRANSMISSION_LINE) {};
  Admitt(double r, double x, double sh, double tap, double angle_phi) : r(r), x(x), sh(sh), tap(tap), angle_phi(angle_phi), type(TRANSMISSION_LINE), to(0), from(0) {};
  Admitt(double r, double x, double sh, double tap, double angle_phi, int type) : r(r), x(x), sh(sh), tap(tap), angle_phi(angle_phi), type(type), to(0), from(0) {};
  Admitt(container::vector<string> data) :
    r(atof(data.at(0).c_str())), x(atof(data.at(1).c_str())), sh(atof(data.at(2).c_str()) / 2), tap(atof(data.at(3).c_str())),
    angle_phi(atof(data.at(4).c_str())), type(atoi(data.at(5).c_str())), to(atoi(data.at(6).c_str())), from(atoi(data.at(7).c_str())) {};
  Admitt(double r, double x) : r(r), x(x), sh(0), tap(1), angle_phi(0), type(TRANSMISSION_LINE) {};
  Admitt() : r(0), x(0), sh(0), tap(1), angle_phi(0), type(0), to(0), from(0) {};
  void SetR(double r);
  void SetX(double x);
  void SetSh(double sh);
  void SetTap(double tap);
  void SetType(int type);
  void SetAngle(double angle);

  double GetR();
  double GetX();
  double GetSh();
  double GetTap();
  double GetAngle();
  int GetType();
  int GetTo();
  int GetFrom();
};

#endif /* ADMITT_H_ */
