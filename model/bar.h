//#include <vector>
#include <map>
#include "node.h"
#include <string>

#define SLACK 3
#define LOAD 2
#define GENERATION 1

#define VOLTAGE 100
#define A_POWER 200
#define R_POWER 300

using namespace std;

class Bar {
private:
  double angle;
  double voltage;
  double aPower;
  double aPowerG;
  double aPowerL;
  double rPower;
  double rPowerG;
  double rPowerL;
  int type;
  int id;
  double c;
  double s;
  double bSh;

  double calcV;
  double calcQ;
  double calcP;

  map<int, Bar*> neighbors;
  map<int, Node*> impd;
  /*vector<Bar> neighbors;
  vector<Node> impd;*/

public:
  Bar(double angle, double voltage, double aPower, double rPower, int type, int id, double bSh) :
      angle(angle), voltage(voltage), aPower(aPower), aPowerG(0), aPowerL(0), rPower(rPower), rPowerG(0), rPowerL(0), type(type), id(id),
      bSh(bSh)
  {};

  Bar(double angle, double voltage, double aPower, double rPower, int type, int id, double bSh, int description):
      angle(angle), voltage(voltage), aPower(aPower), aPowerG(0), aPowerL(0), rPower(rPower), rPowerG(0), rPowerL(0), type(type), id(id),
      bSh(bSh)
  {};

  Bar(double angle, double voltage, double aPowerG, double aPowerL, double rPowerG, double rPowerL, int type, int id, double bSh) :
    angle(angle), voltage(voltage), aPowerG(aPowerG), aPowerL(aPowerL), rPowerG(rPowerG), rPowerL(rPowerL), type(type), id(id), c(0), s(0), bSh(bSh)
  {
    aPower = aPowerG - aPowerL;
    rPower = rPowerG - rPowerL;
  };

  Bar(double angle, double voltage, double aPowerG, double aPowerL, double rPowerG, double rPowerL, int type, int id, double bSh, int description) :
    angle(angle), voltage(voltage), aPowerG(aPowerG), aPowerL(aPowerL), rPowerG(rPowerG), rPowerL(rPowerL), type(type), id(id), c(0), s(0), bSh(bSh)
  {
    aPower = aPowerG - aPowerL;
    rPower = rPowerG - rPowerL;
  }

  ~Bar();

  double GetAngle();
  double GetVoltage();
  double GetAPower();
  double GetRPower();
  double GetAPowerG();
  double GetAPowerL();
  double GetRPowerG();
  double GetRPowerL();
  double GetC();
  double GetS();
  double GetBSh();
  double GetEst(int variable);

  int GetType();
  int GetId();
  //int GetBSh();

  void SetAngle(double angle);
  void SetVoltage(double voltage);
  void SetAPower(double aPower);
  void SetRPower(double rPower);
  void SetAPowerG(double aPowerG);
  void SetAPowerL(double aPowerL);
  void SetRPowerG(double rPowerG);
  void SetRPowerL(double rPowerL);
  void SetC(double c);
  void SetS(double s);
  void SetBSh(double bSh);
  void SetEst(int variable, double value);

  void AddN(Bar* n, Node* i);
  Node * HasN(int w);
  map<int, Bar*> GetNs();
  map<int, Node*> GetWeight();
  Node * GetEdge(int n);
  void Clear();
};
