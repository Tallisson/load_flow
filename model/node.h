#ifndef NODE_H_
#define NODE_H_

#include "define.h"

namespace load
{

#define MIN_TAP 100
#define MAX_TAP 200
#define MIN_PHI 300
#define MAX_PHI 400

class Node{
private:
  double c;
  double s;
  double sh;
  double tap;
  int type;
  double angle_phi;

  double min_lim_tap;
  double max_lim_tap;
  double min_lim_phi;
  double max_lim_phi;
  int from;
  int to;
  int crt_bar;

public:
  Node(double c, double s, int from, int to):
    c(c), s(s), sh(0), tap(1), type(TRANSMISSION_LINE),
    angle_phi(0), min_lim_tap(0), max_lim_tap(0), min_lim_phi(0),
    max_lim_phi(0), from(from), to(to), crt_bar(0)
  {};

  Node(double c, double s, double sh, int from, int to):
    c(c), s(s), sh(sh), tap(1), type(TRANSMISSION_LINE),
    angle_phi(0), min_lim_tap(0), max_lim_tap(0),
    min_lim_phi(0), max_lim_phi(0), from(from), to(to), crt_bar(0)
  {};

  Node(double c, double s, double sh, double tap, int type,
       double angle_phi, double min_lim, double max_lim, int from, int to, int crt_bar):
    c(c), s(s), sh(sh), tap(tap), type(type),
    angle_phi(angle_phi), from(from), to(to), crt_bar(crt_bar)
  {
    if(type == VARIABLE_TAP_MVAR || type == VARIABLE_TAP_VC)
    {
      min_lim_tap = min_lim;
      max_lim_tap = max_lim;
      min_lim_phi = max_lim_phi = 0;
    }

    if(type == VARIABLE_PHASE_ANGLE)
    {
      min_lim_tap = max_lim_tap = 0;
      min_lim_phi = min_lim;
      max_lim_phi = max_lim;
    }
  };

  Node(double c, double s, double sh, double tap, int type, double angle_phi,
       double min_tap, double max_tap, double min_phi,
       double max_phi, int from, int to, int crt_bar):
    c(c), s(s), sh(sh), tap(tap), type(type),
    angle_phi(angle_phi), min_lim_tap(min_tap), max_lim_tap(max_tap), min_lim_phi(min_phi),
    max_lim_phi(max_phi), from(from), to(to), crt_bar(crt_bar)
  {
  };

  Node(): c(0), s(0), sh(0), tap(1), type(TRANSMISSION_LINE), angle_phi(0), from(from), to(to), crt_bar(0)
  {};

  virtual ~Node();
  double GetS();
  double GetC();
  double GetSh();
  double GetTap();
  double GetPhi();
  double GetLim(int var);
  int GetId();
  int GetType();
  int GetCrtBar();

  void SetS(double s);
  void SetC(double c);
  void SetSh(double sh);
  void SetTap(double t);
  void SetPhi(double angle);
  void SetBar(int bar);
  void SetLim(int var, double value);
  bool Exist();

  int GetFrom();
  int GetTo();
};
}

#endif /* NODE_H_ */
