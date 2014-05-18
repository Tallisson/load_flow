//#include <vector>
#include <map>
#include "node.h"

#define SLACK 3
#define LOAD 2
#define GENERATION 1

using namespace std;

class Bar {
private:
  double angle;
  double voltage;
  double aPower;
  double rPower;
  int type;
  int id;
  double c;
  double s;

  map<int, Bar*> neighbors;
  map<int, Node*> impd;
  /*vector<Bar> neighbors;
  vector<Node> impd;*/

public:
  Bar(double angle, double voltage, double aPower, double rPower, int type, int id) :
      angle(angle), voltage(voltage), aPower(aPower), rPower(rPower), type(type), id(id)
  {};
  Bar(double angle, double voltage, double aPower, double rPower, int type, int id, double c, double s) :
      angle(angle), voltage(voltage), aPower(aPower), rPower(rPower), type(type), id(id), c(c), s(s)
  {};

  Bar():
    angle(0), voltage(0), aPower(0), rPower(0), type(0), id(0), c(0), s(0)
  {};

  double GetAngle();
  double GetVoltage();
  double GetAPower();
  double GetRPower();
  double GetC();
  double GetS();

  int GetType();
  int GetId();

  void SetAngle(double angle);
  void SetVoltage(double voltage);
  void SetAPower(double aPower);
  void SetRPower(double rPower);
  void SetC(double c);
  void SetS(double s);
  void AddN(Bar* n, Node* i);
  Node * HasN(int w);
};
