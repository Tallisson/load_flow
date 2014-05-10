#include <vector>
#include "node.h"

#define SLACK 0
#define LOAD 1
#define GENERATION 2

using namespace std;

class Bar {
private:
  double angle;
  double voltage;
  double aPower;
  double rPower;
  int type;
  int id;

  vector<Bar> neighbors;
  vector<Node> impd;

public:
  Bar(double angle, double voltage, double aPower, double rPower, int type, int id) :
      angle(angle), voltage(voltage), aPower(aPower), rPower(rPower), type(type), id(id)
  {};

  Bar() {};

  double GetAngle();
  double GetVoltage();
  double GetAPower();
  double GetRPower();

  int GetType();
  int GetId();

  void SetAngle(double angle);
  void SetVoltage(double voltage);
  void SetAPower(double aPower);
  void SetRPower(double rPower);
  void AddN(Bar n, Node i);
  Node * HasN(int w);
};
