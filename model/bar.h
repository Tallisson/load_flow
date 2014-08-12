#ifndef BAR_H_
#define BAR_H_

#include <boost/container/map.hpp>
#include "node.h"
#include <string>

using namespace boost;

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
  double rPower;
  double aPowerL;
  double rPowerL;
  double aPowerG;
  double rPowerG;
  int type;
  int id;
  double c;
  double s;
  double bSh;

  double calcV;
  double calcQ;
  double calcP;

  container::map<int, Bar*> neighbors;
  container::map<int, Node*> impd;

public:
  Bar(double angle, double voltage, double aPower, double rPower, int type, int id, double bSh) :
      angle(angle), voltage(voltage), aPower(aPower), rPower(rPower), aPowerL(0), rPowerL(0), aPowerG(0), rPowerG(0), type(type), id(id),
      bSh(bSh)
  {};

  Bar(double angle, double voltage, double aPowerL, double rPowerL, double aPowerG, double rPowerG, int type, int id, double bSh) :
    angle(angle), voltage(voltage), aPowerL(aPowerL), rPowerL(rPowerL), aPowerG(aPowerG), rPowerG(rPowerG), type(type), id(id), c(0), s(0), bSh(bSh)
  {
    aPower = aPowerG - aPowerL;
    rPower = rPowerG - rPowerL;
  };

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
  container::map<int, Bar*> GetNs();
  container::map<int, Node*> GetWeight();
  Node * GetEdge(int n);
  void Clear();
};

#endif /* BAR_H_ */
