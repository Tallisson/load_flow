#include "define.h"

class Node{
private:
  double c;
  double s;
  double sh;
  double tap;
  int type;
  double angle_phi;

public:
  Node(double c, double s): c(c), s(s), sh(0), tap(1), type(TRANSMISSION_LINE), angle_phi(0)
  {};
  Node(double c, double s, double sh): c(c), s(s), sh(sh), tap(1), type(TRANSMISSION_LINE), angle_phi(0) {};
  Node(double c, double s, double sh, double tap, double angle_phi): c(c), s(s), sh(sh), tap(tap), angle_phi(angle_phi)
  {};
  Node(): c(0), s(0), sh(0), tap(1), type(TRANSMISSION_LINE), angle_phi(0) {};

  double GetS();
  double GetC();
  double GetSh();
  double GetTap();
  double GetPhi();
  int GetId();
  int GetType();

  void SetS(double s);
  void SetC(double c);
  void SetSh(double sh);
  void SetTap(double t);
  void SetPhi(double angle);

  bool Exist();
};
