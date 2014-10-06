#ifndef ADMITT_H_
#define ADMITT_H_

#include "define.h"
#include <boost/container/vector.hpp>
#include <boost/cstdlib.hpp>

using namespace std;
using namespace boost;

namespace load
{

class Admitt {
private:
  int from;
  int to;
  double r;
  double x;
  double sh;
  double tap;
  double angle_phi;
  int type;
  int min_lim;
  int max_lim;
  int crt_bar;

public:
  Admitt(double r, double x, double sh):
    from(0), to(0), r(r), x(x), sh(sh), tap(1),
    angle_phi(0), type(TRANSMISSION_LINE),
    min_lim(0), max_lim(0), crt_bar(-1)
  {};
  Admitt(double r, double x, double sh, double tap, double angle_phi) :
    from(0), to(0), r(r), x(x), sh(sh), tap(tap),
    angle_phi(angle_phi), type(TRANSMISSION_LINE),
    min_lim(0), max_lim(0), crt_bar(-1)
  {};
  Admitt(double r, double x, double sh, double tap, double angle_phi, int type) :
    from(0), to(0), r(r), x(x), sh(sh), tap(tap),
    angle_phi(angle_phi), type(TRANSMISSION_LINE),
    min_lim(0), max_lim(0), crt_bar(-1)
  {};

  Admitt(container::vector<string> data) {
    from = (atof(data.at(0).c_str()));
    to = (atof(data.at(1).c_str()));
    r = (atof(data.at(2).c_str()));
    x = (atof(data.at(3).c_str()));
    sh = (atof(data.at(4).c_str()) / 2);
    tap = (atof(data.at(5).c_str()));
    angle_phi = (atof(data.at(6).c_str()));
    type = (atoi(data.at(7).c_str()));
    min_lim = (atof(data.at(8).c_str()));
    max_lim = (atof(data.at(9).c_str()));
    crt_bar = (atoi(data.at(10).c_str()));
  };

  Admitt(double r, double x) :
    from(0), to(0), r(r), x(x), sh(0), tap(1), angle_phi(0),
    type(TRANSMISSION_LINE), min_lim(0), max_lim(0)
  {};

  Admitt() :
    from(0), to(0), r(0), x(0), sh(0), tap(1), angle_phi(0),
    type(TRANSMISSION_LINE), min_lim(0), max_lim(0)
  {};

  void SetR(double r);
  void SetX(double x);
  void SetSh(double sh);
  void SetTap(double tap);
  void SetType(int type);
  void SetAngle(double angle);
  void SetMaxLim(double max);
  void SetMinLim(double min);

  double GetR();
  double GetX();
  double GetSh();
  double GetTap();
  double GetAngle();
  int GetType();
  int GetTo();
  int GetFrom();
  double GetMaxLim();
  double GetMinLim();
  int GetCrtBar();
};
}
#endif /* ADMITT_H_ */
