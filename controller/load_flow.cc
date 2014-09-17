#include "load_flow.h"

using namespace std;
using namespace boost;

#define ANGLE_INIT 0
#define VOLTAGE_INIT 1
#define ERROR 0.0001
#define S_BASE 100

LoadFlow::LoadFlow(double error):
    numB(0), nPV(0), nPQ(0), cont(0), nLT(0), nTAP_Fixed(0), nTap_VC(0), nTap_MVAR(0), nTAP_PHASE(0),
    error(error), sBase(S_BASE), use_base(true), verbose(true), s_alpha(1)
{
  bars = new Graph();
  report = new Report();
}

LoadFlow::LoadFlow(double error, double sBase):
    numB(0), nPV(0), nPQ(0), cont(0), error(error), sBase(sBase), use_base(true), verbose(true), s_alpha(1)
{
  bars = new Graph();
}

LoadFlow::LoadFlow():
    numB(0), nPV(0), nPQ(0), cont(0), nLT(0), nTAP_Fixed(0), nTap_VC(0), nTap_MVAR(0), nTAP_PHASE(0),
    error(ERROR), use_base(true), s_alpha(1)
{
  bars = new Graph();
}

LoadFlow::~LoadFlow() {
  delete bars;
  delete report;
  delete description;
}

void LoadFlow::AddBar(Bar* bar) {
  if(use_base) {
    bar->SetAPower(bar->GetAPower() / sBase);
    bar->SetRPower(bar->GetRPower() / sBase);

    bar->SetAPowerG(bar->GetAPowerG() / sBase);
    bar->SetRPowerG(bar->GetRPowerG() / sBase);
    bar->SetAPowerL(bar->GetAPowerL() / sBase);
    bar->SetRPowerL(bar->GetRPowerL() / sBase);
    bar->SetBSh(bar->GetBSh() / sBase);
  }

  if(verbose) {
    cout << "Bar " << bar->GetId() << ": (P, Q, Bsh): " << bar->GetAPowerG() << ", " << bar->GetRPowerG() << ", " << bar->GetBSh() << endl;
  }

  if(bar->GetType() == GENERATION) {
    nPV++;
    ord.insert(pair<int, int>(bar->GetId(), cont++));
  }

  if(bar->GetType() == LOAD) {
    int id = bar->GetId();
    ord.insert(pair<int, int>(id, cont++));
    ordPQ.insert(pair<int, int>(id, nPQ));
    ++nPQ;
  }

  bars->AddV(bar);
  numB++;
}

Bar * LoadFlow::GetBar(int v) {
  return bars->at(v);
}

Graph * LoadFlow::GetGraph() {
  return this->bars;
}

void LoadFlow::AssocBars(Bar* v, Bar* w, Node* node) {
  switch(node->GetType()) {
  case FIXED_TAP:
    nTAP_Fixed++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 1) );
    break;
  case VARIABLE_TAP_VC:
    nTap_VC++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 1) );
    break;
  case VARIABLE_TAP_MVAR:
    nTap_MVAR++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 1) );
    break;
  case VARIABLE_PHASE_ANGLE:
    nTAP_PHASE++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 0) );
    break;
  default:
    break;
  }

  bars->AddEdge(v, w, node);
}

void LoadFlow::AssocBars(Bar* v, Bar* w, Admitt* admitt) {
  Node* node = bars->AddEdge(v, w, admitt);

  switch(admitt->GetType()) {
  case TRANSMISSION_LINE:
    nLT++;
    break;
  case FIXED_TAP:
    nTAP_Fixed++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 1) );
    break;
  case VARIABLE_TAP_VC:
    nTap_VC++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 1) );
    break;
  case VARIABLE_TAP_MVAR:
    nTap_MVAR++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 1) );
    break;
  case VARIABLE_PHASE_ANGLE:
    nTAP_PHASE++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 0) );
    break;
  default:
    break;
  }
}

void LoadFlow::AssocBars(Admitt* admitt) {
  Node* node = bars->AddEdge(bars->at(admitt->GetFrom()), bars->at(admitt->GetTo()), admitt);

  switch(admitt->GetType()) {
  case TRANSMISSION_LINE:
    nLT++;
    break;
  case FIXED_TAP:
    nTAP_Fixed++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 1) );
    break;
  case VARIABLE_TAP_VC:
    nTap_VC++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 1) );
    break;
  case VARIABLE_TAP_MVAR:
    nTap_MVAR++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 1) );
    break;
  case VARIABLE_PHASE_ANGLE:
    nTAP_PHASE++;
    estCrtlVar.insert( std::pair<Node*, double>(node, 0) );
    break;
  default:
    break;
  }
}

Node * LoadFlow::GetEdge(int v, int w) {
  Node * edge = bars->GetEdge(v, w);
  return edge;
}

void LoadFlow::SetSimetric(bool s) {
  bars->SetSimetric(s);
}

void LoadFlow::initState() {
  Bar * bar;
  container::map<int, Bar*> nodes = bars->GetBars();

  for (container::map<int, Bar*>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
    bar = it->second;

    if(bar->GetType() != SLACK) {
      bar->SetAngle(ANGLE_INIT);
    }

    if(bar->GetType() == LOAD) {
      bar->SetVoltage(VOLTAGE_INIT);
    }
  }
}

void LoadFlow::initState(double aInitial, double vInitial) {
  Bar * bar;
  container::map<int, Bar*> nodes = bars->GetBars();

  for (container::map<int, Bar*>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
    bar = it->second;

    if(bar->GetType() != SLACK) {
      bar->SetAngle(aInitial);
    }

    if(bar->GetType() == LOAD) {
      bar->SetVoltage(vInitial);
    }
  }
}

void LoadFlow::initialize() {
  int s = nPV + (nPQ << 1);

  jacobian = zeros<mat>(s, s);
  estP = zeros<vec>(s);
  estS = zeros<vec>(s);
  diffP = zeros<vec>(s);
  diffS = zeros<vec>(s);
  initState();
}

void LoadFlow::initialize(const char* file) {
  Utils* ut = Utils::GetInstance();
  description = ut->ProcessFile(file);
  ut->Delete();

  (description->s_base > 0) ? this->sBase = description->s_base : 0;

  for(unsigned i = 0; i < description->bars.size(); i++) {
    AddBar(description->bars.at(i));
  }

  container::vector<Admitt*> branchs = description->admmits;
  for(unsigned i = 0; i < branchs.size(); i++) {
    this->AssocBars(branchs.at(i));
  }

  this->initialize();
}

void LoadFlow::initJ() {
  int s = nPV + (nPQ << 1);
  jacobian = zeros<mat>(s, s);
}

void LoadFlow::updateState() {
  Bar * barK;

  container::map<int, int>::iterator it=ord.begin();
  for(int k = 0; it != ord.end(); k++) {
    barK = bars->at(it->first);

    if(barK->GetType() != SLACK)
      barK->SetAngle(barK->GetAngle() + diffS(k));

    if(barK->GetType() == LOAD) {
      int i = ordPQ.at(barK->GetId()) + (nPV + nPQ);
      barK->SetVoltage(barK->GetVoltage() + diffS(i));
    }

    it++;
  }
}

bool LoadFlow::nextIterate() {
  vec t = abs(diffP);
  max_error = max(t);
  return  max_error > error;
}

void LoadFlow::mismatches() {
  double theta;
  container::map<int, Bar*> nodes = bars->GetBars();

  container::map<int, int>::iterator it=ord.begin();
  for(int k = 0; it != ord.end(); k++) {
    Bar * barK = bars->at(it->first);
    Bar * barM;

    if(barK->GetType() != SLACK) {
      double vK = barK->GetVoltage();
      diffP(k) = barK->GetAPower();

      container::map<int, Node*> neighbors = barK->GetWeight();
      Node* edge;
      int m;
      for (container::map<int, Node*>::iterator it=neighbors.begin(); it!=neighbors.end(); ++it) {
        m = it->first;
        barM = bars->at(m);
        edge = it->second;
        theta = barK->GetAngle() - barM->GetAngle();

        // (g(km)*V(k)^2 - V(k)*V(m)*(g(km)*cos(akm)+b(km)*sin(akm)));
        if(edge->GetFrom() == barK->GetId())
        {
          diffP(k) = diffP(k) - (edge->GetC() * pow((vK * 1/edge->GetTap()), 2) - (vK * 1/edge->GetTap()) * barM->GetVoltage() *
                   (edge->GetC() * cos(theta - edge->GetPhi()) + edge->GetS() * sin(theta - edge->GetPhi())));
        } else if(edge->GetFrom() == barM->GetId())
        {
          diffP(k) = diffP(k) - (edge->GetC() * pow(vK, 2) - (vK * 1/edge->GetTap()) * barM->GetVoltage() *
                             (edge->GetC() * cos(theta + edge->GetPhi()) + edge->GetS() * sin(theta + edge->GetPhi())));
        }
      }
    }

    if(barK->GetType() == LOAD) {
      int i = ordPQ.at(barK->GetId()) + (nPQ + nPV);
      double vK = barK->GetVoltage();

      // Incluindo Susceptância Nodal na Potência reativa
      // Qg(k) + Bsh(k)*V(k)^2 - Qc(k)
      diffP(i) = (barK->GetRPower() + barK->GetBSh() * pow(vK, 2));
      container::map<int, Node*> neighbors = barK->GetWeight();

      Node* edge;
      int m;
      for (container::map<int, Node*>::iterator it=neighbors.begin(); it!=neighbors.end(); ++it) {
        m = it->first;
        barM = bars->at(m);
        edge = it->second;
        theta = barK->GetAngle() - barM->GetAngle();

        // mis(nb-1+ordPQ(k)) = mis(nb-1+ordPQ(k)) - (-(b(km)+bsh(km))*V(k)^2 + V(k)*V(m)*(b(km)*cos(akm)-g(km)*sin(akm)));
        if(edge->GetFrom() == barK->GetId())
        {
          diffP(i) -= -((edge->GetS() * pow(1/edge->GetTap(), 2)) + edge->GetSh())  * pow(vK, 2) + (vK * 1/edge->GetTap()) * barM->GetVoltage() *
                    (edge->GetS() * cos(theta - edge->GetPhi()) - edge->GetC() * sin(theta - edge->GetPhi()));
        } else if(edge->GetFrom() == barM->GetId())
        {
          diffP(i) -= -(edge->GetS() + edge->GetSh())  * pow(vK, 2) + (vK * 1/edge->GetTap()) * barM->GetVoltage() *
                              (edge->GetS() * cos(theta + edge->GetPhi()) - edge->GetC() * sin(theta + edge->GetPhi()));
        }
      }

    }

    it++;
  }

}

void LoadFlow::solveSys() {
  mat m = inv(jacobian);
  diffS = m*-diffP;
}

void LoadFlow::DpDer() {
  Bar * barK, * barM;

  container::map<int, int>::iterator it=ord.begin();
  int noSlack = nPQ + nPV;
  for(int k = 0; k < noSlack; k++) {
    barK = bars->at(it->first);
    int m = 0;

    if(barK->GetType() != SLACK) {
      container::map<int, Bar*> neighbors = barK->GetNs();
      for(container::map<int, Bar*>::iterator itN=neighbors.begin(); itN!=neighbors.end(); ++itN) {
        barM = itN->second;
        double theta = barK->GetAngle() - barM->GetAngle();
        Node * edge = barK->GetEdge(barM->GetId());

        // dPk em relação a 'ak '.
        // -V(k)*V(m)*(g(km)*sin(akm)-b(km)*cos(akm)) + Jac(k-1, k-1);
        if(edge->GetFrom() == barK->GetId())
        {
          jacobian(k, k) = -(barK->GetVoltage()* 1/edge->GetTap()) * barM->GetVoltage() *
                            (edge->GetC() * sin(theta - edge->GetPhi()) - edge->GetS() * cos(theta - edge->GetPhi())) + jacobian(k, k);
        } else if(edge->GetFrom() == barM->GetId())
        {
          jacobian(k, k) = -barK->GetVoltage() * barM->GetVoltage() * (edge->GetC() * sin(theta - edge->GetPhi()) -
                            edge->GetS() * cos(theta - edge->GetPhi())) + jacobian(k, k);
        }

        // dPk em relação a 'am' (exceto quando m for a barra slack).
        if(barM->GetType() != SLACK) {
          // V(k)*V(m)*(g(km)*sin(akm)-b(km)*cos(akm));
          //cout << "M: " << m << endl;
          m = ord.at(barM->GetId());
          jacobian(k, m) = barK->GetVoltage() * barM->GetVoltage() * (edge->GetC() * sin(theta) - edge->GetS() * cos(theta));
        }

        // dPk em relação a 'vk'.
        if(barK->GetType() == LOAD) {
          // -2*g(km)*V(k) + V(m)*(g(km)*cos(akm)+b(km)*sin(akm)) + Jac(k-1, nb-1+ordPQ(k));
          m = noSlack + ordPQ.at(barK->GetId());
          if(edge->GetFrom() == barK->GetId())
          {
            jacobian(k, m) = -2 * edge->GetC() * (barK->GetVoltage() * 1 / edge->GetTap()) + barM->GetVoltage() *
                             (edge->GetC() * cos(theta - edge->GetPhi()) + edge->GetS() * sin(theta - edge->GetPhi())) + jacobian(k, m);
          } else if(edge->GetFrom() == barM->GetId())
          {
            jacobian(k, m) = -2 * edge->GetC() * (barK->GetVoltage() * 1 / edge->GetTap()) + barM->GetVoltage() *
                             (edge->GetC() * cos(theta + edge->GetPhi()) + edge->GetS() * sin(theta + edge->GetPhi())) + jacobian(k, m);
          }
        }

        // dPk em relação a 'vm'.
        if(barM->GetType() == LOAD) {
          // V(k)*(g(km)*cos(akm)+b(km)*sin(akm));
          m = noSlack + ordPQ.at(barM->GetId());

          if(edge->GetFrom() == barK->GetId())
          {
            jacobian(k, m) = (barK->GetVoltage() * edge->GetTap()) *
                             (edge->GetC() * cos(theta - edge->GetPhi()) + edge->GetS() * sin(theta - edge->GetPhi()));
          } else if(edge->GetFrom() == barM->GetId()) {
            jacobian(k, m) = (barK->GetVoltage() * edge->GetTap()) *
                               (edge->GetC() * cos(theta + edge->GetPhi()) + edge->GetS() * sin(theta + edge->GetPhi()));
          }
        }
      }
    }

    it++;
  }
}

void LoadFlow::DqDer() {
  Bar * barK, * barM;

  container::map<int, int>::iterator it=ordPQ.begin();
  int s = nPV+ (nPQ << 1);
  int noSlack = nPQ + nPV;
  for(int k = nPQ + nPV; k < s; ) {
    barK = bars->at(it->first);
    int m = 0;

    if(barK->GetType() == LOAD) {
      container::map<int, Bar*> neighbors = barK->GetNs();
      //std::map<int, int>::iterator it = ord.find(barK->GetId());

      for(container::map<int, Bar*>::iterator itN=neighbors.begin(); itN!=neighbors.end(); ++itN) {
        barM = itN->second;
        double theta = barK->GetAngle() - barM->GetAngle();
        Node * edge = barK->GetEdge(barM->GetId());

        // dQk em relaçao a 'ak'.
        // V(k)*V(m)*(b(km)*sin(akm)+g(km)*cos(akm)) + Jac(nb-1+ordPQ(k), k-1);
        m = ord.at(barK->GetId());
        if(edge->GetFrom() == barK->GetId())
        {
          jacobian(k, m) = (barK->GetVoltage() * 1/edge->GetTap()) * barM->GetVoltage() *
                            (edge->GetS() * sin(theta - edge->GetPhi()) + edge->GetC() * cos(theta - edge->GetPhi())) + jacobian(k, m);
        } else if(edge->GetFrom() == barM->GetId())
        {
          jacobian(k, m) = (barK->GetVoltage() * 1/edge->GetTap()) * barM->GetVoltage() *
                            (edge->GetS() * sin(theta + edge->GetPhi()) + edge->GetC() * cos(theta + edge->GetPhi())) + jacobian(k, m);
        }

        // dQk em relaçao a 'am' (exceto quando m for a barra slack).
        if(barM->GetType() != SLACK) {
          // -V(k)*V(m)*(g(km)*cos(akm)+b(km)*sin(akm));
          m = ord.at(barM->GetId());
          if(edge->GetFrom() == barK->GetId())
          {
            jacobian(k, m) = -(barK->GetVoltage() * 1/edge->GetTap()) * barM->GetVoltage() *
                            (edge->GetC() * cos(theta - edge->GetPhi()) + edge->GetS() * sin(theta - edge->GetPhi()));
          } else if(edge->GetFrom() == barM->GetId())
          {
            jacobian(k, m) = -(barK->GetVoltage() * 1/edge->GetTap()) * barM->GetVoltage() *
                              (edge->GetC() * cos(theta + edge->GetPhi()) + edge->GetS() * sin(theta + edge->GetPhi()));
          }
        }

        // dQk em relaçao a 'vk'.
        // 2*(b(km)+bsh(km))*V(k) - V(m)*(b(km)*cos(akm)-g(km)*sin(akm)) + Jac(nb-1+ordPQ(k), nb-1+ordPQ(k));
        if(edge->GetFrom() == barK->GetId())
        {
          jacobian(k, k) = 2 * (edge->GetS()+edge->GetSh()) * (barK->GetVoltage() * 1/edge->GetTap()) - barM->GetVoltage() *
                              (edge->GetS() * cos(theta - edge->GetPhi()) - edge->GetC() * sin(theta - edge->GetPhi())) + jacobian(k, k);
        } else if(edge->GetFrom() == barM->GetId())
        {
          jacobian(k, k) = 2 * (edge->GetS()+edge->GetSh()) * (barK->GetVoltage() * 1/edge->GetTap()) -
                                        barM->GetVoltage() * (edge->GetS() * cos(theta + edge->GetPhi()) - edge->GetC() * sin(theta + edge->GetPhi())) + jacobian(k, k);
        }
        // dQk em relacao a 'vm'.
        if(barM->GetType() == LOAD) {
          // -V(k)*(b(km)*cos(akm)-g(km)*sin(akm));
          /*m = ord.at(barM->GetId());
          int index = m + nPQ;
          if(index < nPV + nPQ) {
            index++;
          }*/
          m = ordPQ.at(barM->GetId()) + noSlack;
          if(edge->GetFrom() == barK->GetId())
          {
            jacobian(k, m) = -(barK->GetVoltage()*1/edge->GetTap()) * (edge->GetS() * cos(theta - edge->GetPhi()) -
                             edge->GetC() * sin(theta - edge->GetPhi()));
          } else if(edge->GetFrom() == barM->GetId())
          {
            jacobian(k, m) = -(barK->GetVoltage()*1/edge->GetTap()) * (edge->GetS() * cos(theta + edge->GetPhi()) -
                             edge->GetC() * sin(theta + edge->GetPhi()));
          }
        }
      }

      // dQk em relaçao a 'vk' (continuação).
      // 2*Bsh(k)*V(k)
      jacobian(k, k) = 2*barK->GetBSh() * barK->GetVoltage() + jacobian(k, k);
      k++;
    }

    it++;
  }
}

void LoadFlow::calcJ() {
  initJ();
  DpDer();
  DqDer();
}

void LoadFlow::calcS2() {
  Bar * barK, * barM;
  container::map<int, Bar*> nodes = bars->GetBars();

  for(container::map<int, Bar*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
    barK = it->second;

    if(barK->GetType() == SLACK) {
      double sum = 0;
      container::map<int, Bar*> neighbors = barK->GetNs();

      for(container::map<int, Bar*>::iterator itN = neighbors.begin(); itN != neighbors.end(); itN++) {
        barM = itN->second;
        double theta = barK->GetAngle() - barM->GetAngle();
        Node* edge = barK->GetEdge(barM->GetId());

        // (gkm(km)*V(k)^2 - V(k)*V(m)*(gkm(km)*cos(akm)+bkm(km)*sin(akm)));
        double vK = barK->GetVoltage();
        if(edge->GetFrom() == barK->GetId())
        {
          sum += (edge->GetC() * pow((vK * 1/edge->GetTap()), 2) - (vK * 1/edge->GetTap()) *
                  barM->GetVoltage() * (edge->GetC() * cos(theta - edge->GetPhi()) + edge->GetS() * sin(theta - edge->GetPhi())) );
        } else if(edge->GetFrom() == barM->GetId())
        {
          sum += (edge->GetC() * pow(vK, 2) - (vK * 1/edge->GetTap()) *
                 barM->GetVoltage() * (edge->GetC() * cos(theta + edge->GetPhi()) + edge->GetS() * sin(theta + edge->GetPhi())) );
        }
      }
      barK->SetAPower(sum);

    }

    if(barK->GetType() != LOAD) {
      double sum = 0;
      container::map<int, Bar*> neighbors = barK->GetNs();

      for(container::map<int, Bar*>::iterator itN = neighbors.begin(); itN != neighbors.end(); itN++) {
        barM = itN->second;
        double theta = barK->GetAngle() - barM->GetAngle();
        Node* edge = barK->GetEdge(barM->GetId());

        // (-(bkm(km)+bkm_sh(km))*V(k)^2 + V(k)*V(m)*(bkm(km)*cos(akm)-gkm(km)*sin(akm)));
        double vK = barK->GetVoltage();
        if(edge->GetFrom() == barK->GetId())
        {
          sum += (-(edge->GetS() + edge->GetSh()) * pow((vK * 1/edge->GetTap()),2) + (vK * 1/edge->GetTap()) * barM->GetVoltage() *
                  (edge->GetS() * cos(theta - edge->GetPhi()) - edge->GetC() * sin(theta - edge->GetPhi())) );
        } else if(edge->GetFrom() == barM->GetId())
        {
          sum += (-(edge->GetS() + edge->GetSh()) * pow(vK,2) + (vK * 1/edge->GetTap()) * barM->GetVoltage() *
                  (edge->GetS() * cos(theta + edge->GetPhi()) - edge->GetC() * sin(theta + edge->GetPhi())) );
        }
      }
      barK->SetRPower(sum);

    }
  }
}

int LoadFlow::Execute() {
  return Execute("");
}

int LoadFlow::Execute(const char * file) {
  int counter = 0;
  if(!boost::iequals(string(file), string(""))) {
    initialize(file);
  } else {
    initialize();
  }

  mismatches();

  if(verbose == true) {
    for(int i = 0; i < numB; i++) {
      Bar * tmp = bars->at(i+1);
      cout << "Bar(" << tmp->GetId() << ")=> (v, a): " << tmp->GetVoltage()  << ", "  << tmp->GetAngle()<< endl;
    }

    cout << "Erro: " << endl << diffP << endl;
  }

  while(true) {
    calcJ();
    solveSys();
    updateState();
    counter++;
    mismatches();
    setControlVariables();

    if(verbose == true) {
      cout << "Jac" << endl << jacobian << endl;

      cout << "dx" << endl << diffS << endl;

      for(int i = 0; i < numB; i++) {
        Bar * tmp = bars->at(i+1);
        cout << "Bar(" << tmp->GetId() << ")=> (v, a): " << tmp->GetVoltage()  << ", "  << tmp->GetAngle()<< endl;
      }

      cout << "Erro: " << endl << diffP << endl;
    }

    if(nextIterate() == false) {
      break;
    }
  }

  calcS2();
  if(verbose == true) {
    cout << "O processo convergiu com " << counter << " iterações\n" << endl;

    for(int i = 0; i < numB; i++) {
      Bar * b = bars->at(i+1);

      //cout << "Barra(" << b->GetId()+1 << "): Estado(a, v, p, q): (" << b->GetAngle() << ", " << b->GetVoltage() << ", " << b->GetAPower() << ", " << b->GetRPower() << ")" << endl;
      printf("Barra(%d)=> Angulo: %.5f, Voltagem: %.5f\n", b->GetId(), b->GetAngle(), b->GetVoltage());
    }
  }

  CalcReport();
  if(verbose == true) {
    cout << endl << "Grandezas Calculdas: " << endl;
    container::map<Bar*, Quantity*> data = report->GetPower();
    for(container::map<Bar*, Quantity*>::iterator it = data.begin(); it != data.end(); it++) {
      Bar* barK = it->first;
      Quantity* qt = it->second;

      cout << "Barra " << barK->GetId() << ": { PG [p.u.] = " << qt->GetAttr(PG) << ", QG [p.u.] = " << qt->GetAttr(QG)
           << ", PC [p.u.] = " << qt->GetAttr(PC) <<  ", QC [p.u.] = " << qt->GetAttr(QC) << " }" << endl;
    }

    cout << endl << "Perdas: " << endl;
    container::map<Node*, Loss*> losses = report->GetLosses();
    cout << "Pkm[p.u.] | " << " Pmk[p.u.] | " << "Perdas | " << "Qkm[p.u.] | " << "Qmk[p.u.]" << endl;
    double loss_total = 0;
    for(container::map<Node*, Loss*>::iterator it = losses.begin(); it != losses.end(); it++) {
      Loss* l = it->second;
      loss_total += l->GetAttr(LOSS);
      double loss = l->GetAttr(LOSS);
      cout << l->GetAttr(P_IN) << " | " << l->GetAttr(P_OUT) << " | " << (loss) << " | " << l->GetAttr(Q_IN) << " | " << l->GetAttr(Q_OUT) << endl;
    }

    cout << "Perda Total: " << loss_total * sBase << endl;
  }

  return counter;
}

void LoadFlow::SetUseBase(bool use_base) {
  this->use_base = use_base;
}

/*
 * Controles implementados:
 * 1) Controle de magnitude de tensão nodal (local e remota) por injeção de reativos
 * 2) Controle de magnitude de tensão nodal (local e remota) por ajuste de tap (transformadores em-fase)
 * 3) Controle de fluxo de potência (transformadores defasadores)
 *
 * Ajuste alternado:
 * correção na variavel de controle = diferença entre valores calculados e especificados da variável controlada * sensibilidade
 */
void LoadFlow::setControlVariables() {
  for( container::map<Node*, double>::iterator it = estCrtlVar.begin(); it != estCrtlVar.end(); it++ ) {
    Node * edge = it->first;
    Bar* crt_bar = bars->at(edge->GetCrtBar());

    // deltaU = alfa * deltaZ
    // deltaZ = z_esp - z_cal
    switch(edge->GetType()) {
    //case FIXED_TAP:

    case VARIABLE_TAP_VC:
    {
      double old_tap = it->second;
      double delta_z = crt_bar->GetEst(VOLTAGE) - crt_bar->GetVoltage();

      if(delta_z > error) {
        double new_tap = old_tap + s_alpha * (delta_z);

        double limit = edge->GetLim(MAX_TAP);
        if(new_tap > limit) {
          new_tap = limit;
        } else {
          limit = edge->GetLim(MIN_TAP);
          if(new_tap < limit) {
            new_tap = limit;
          }
        }

        it->second = new_tap;
      }
      break;
    }
    case VARIABLE_TAP_MVAR:
    {
      double old_tap = it->second;
      double delta_z = crt_bar->GetEst(R_POWER) - crt_bar->GetRPower();
      if(delta_z > error) {
        double new_tap = old_tap + s_alpha * (delta_z);

        double limit = edge->GetLim(MAX_TAP);
        if(new_tap > limit) {
          new_tap = limit;
        } else {
          limit = edge->GetLim(MIN_TAP);
          if(new_tap < limit) {
            new_tap = limit;
          }
        }

        it->second = new_tap;
      }
      break;
    }
    case VARIABLE_PHASE_ANGLE:
    {
      double old_angle = it->second;
      double delta_z = crt_bar->GetEst(A_POWER) - crt_bar->GetAPower();

      if(delta_z > error) {
        double new_angle = old_angle + s_alpha * (delta_z);

        double limit = edge->GetLim(MAX_PHI);
        if(new_angle > limit) {
          new_angle = limit;
        } else {
          limit = edge->GetLim(MIN_PHI);
          if(new_angle < limit) {
            new_angle = limit;
          }
        }

        it->second = new_angle;
      }
      break;
    }
    default:
      break;
    }
  }
}

void LoadFlow::insertLoss(Node* edge, Bar* barK, Bar* barM) {
  double vK = barK->GetVoltage();
  double vM = barM->GetVoltage();
  double theta = barK->GetAngle() - barM->GetAngle();
  double p_km, p_mk, q_km, q_mk, perdas;

  if(edge->GetFrom() == barK->GetId())
  {
    p_km = edge->GetC() * pow((vK * 1/edge->GetTap()), 2) - (vK * 1/edge->GetTap()) * vM *
                  (edge->GetC() * cos(theta - edge->GetPhi()) + edge->GetS() * sin(theta - edge->GetPhi()));
    p_mk = edge->GetC() * pow(vM, 2) - (1/edge->GetTap() * vK) * vM *
                  (edge->GetC() * cos(theta - edge->GetPhi()) - edge->GetS() * sin(theta - edge->GetPhi()));
    q_km = -(pow(edge->GetS() * 1/edge->GetTap(), 2) + edge->GetSh()) * pow(vK, 2) + (1 / edge->GetTap() * vK) * vM *
                  (edge->GetS() * cos(theta - edge->GetPhi()) - edge->GetC() * sin(theta - edge->GetPhi()));
    q_mk = -(edge->GetS() + edge->GetSh()) * pow(vM, 2) + (1/edge->GetTap() * vK) * vM *
                  (edge->GetS() * cos(theta) + edge->GetC() * sin(theta));
    perdas = edge->GetC() * (pow((vK * 1/edge->GetTap()), 2) + pow(vM, 2) - 2 * (1/edge->GetTap() * vK) * vM *
                    cos(theta - edge->GetPhi()));
  } else if(edge->GetFrom() == barM->GetId())
  {
    p_km = edge->GetC() * pow(vK, 2) - (vK * 1/edge->GetTap()) * vM *
                  (edge->GetC() * cos(theta + edge->GetPhi()) + edge->GetS() * sin(theta + edge->GetPhi()));
    p_mk = edge->GetC() * pow(vM, 2) - (1/edge->GetTap() * vK) * vM *
                  (edge->GetC() * cos(theta + edge->GetPhi()) - edge->GetS() * sin(theta + edge->GetPhi()));
    q_km = -(edge->GetS() + edge->GetSh()) * pow((vK * 1/edge->GetTap()), 2) + (1 / edge->GetTap() * vK) * vM *
                  (edge->GetS() * cos(theta + edge->GetPhi()) - edge->GetC() * sin(theta + edge->GetPhi()));
    q_mk = -(edge->GetS() + edge->GetSh()) * pow(vM, 2) + (1/edge->GetTap() * vK) * vM *
                  (edge->GetS() * cos(theta) + edge->GetC() * sin(theta));
    perdas = edge->GetC() * (pow(vK, 2) + pow(vM, 2) - 2 * (1/edge->GetTap() * vK) * vM *
                    cos(theta + edge->GetPhi()));
  }

  Loss * l = new Loss;
  l->SetAttr(P_IN, p_km);
  l->SetAttr(P_OUT, p_mk);
  l->SetAttr(Q_IN, q_km);
  l->SetAttr(Q_OUT, q_mk);
  l->SetAttr(LOSS, perdas);

  report->Insert(edge, l);
}

/*
 * 1) Cálculo de Grandezas: Potência ativa gerada e Potência Reativa Gerada
 * 2) Cálculo de Perdas de Potência:
 *      Pkm(i) = gkm(i)*V(k)^2 - V(k)*V(m)*(gkm(i)*cos(akm)+bkm(i)*sin(akm));
 *      Pmk(i) = gkm(i)*V(m)^2 - V(k)*V(m)*(gkm(i)*cos(akm)-bkm(i)*sin(akm));
 *      Qkm(i) = -(bkm(i)+bkm_sh(i))*V(k)^2 + V(k)*V(m)*(bkm(i)*cos(akm)-gkm(i)*sin(akm));
 *      Qmk(i) = -(bkm(i)+bkm_sh(i))*V(m)^2 + V(k)*V(m)*(bkm(i)*cos(akm)+gkm(i)*sin(akm));
 *
 *      Perdas(i) = gkm(i)*(V(k)^2 + V(m)^2 - 2*V(k)*V(m)*cos(akm));
 */
void LoadFlow::CalcReport() {
  Bar * barK, * barM;
  container::map<int, Bar*> nodes = bars->GetBars();

  for(container::map<int, Bar*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
    barK = it->second;

    /*
     * Para todas as barras do tipo slack (tipo 3):
     * 1) Pg(k) = Pc(k)
     * 2) Pg(k) = Pg(k) + G(km) * (V(k) ^ 2 - V(k) * V(m) *(G(km) * cos(theta) + Bkm * sen(theta)))
     */
    if(barK->GetType() == SLACK) {
      double generate_power = barK->GetAPowerL();
      double reactive_power = -barK->GetBSh() * pow(barK->GetVoltage(), 2) + barK->GetRPowerL();

      Quantity * qt = new Quantity();
      qt->SetAttr(PC, generate_power);
      qt->SetAttr(QC, barK->GetRPowerL());

      container::map<int, Bar*> neighbors = barK->GetNs();
      for(container::map<int, Bar*>::iterator itN = neighbors.begin(); itN != neighbors.end(); itN++) {
        barM = itN->second;
        //double theta = barK->GetAngle() - barM->GetAngle();
        Node* edge = barK->GetEdge(barM->GetId());

        /*double vK = barK->GetVoltage();
        double vM = barM->GetVoltage();*/

        //this->insertLoss(edge, vK, vM, theta);
        this->insertLoss(edge, barK, barM);
      }
      qt->SetAttr(PG, generate_power);

      qt->SetAttr(QG, reactive_power);

      report->Insert(barK, qt);
    }

    /*
     * Para todas as barras diferentes de LOAD:
     * 1) Qg(k) = -Bsh(k) * V(k)^2 + Qc(k);
     * 2) Qg(k) = Qg(k) + (-(bkm(km)+bkm_sh(km))*V(k)^2 + V(k)*V(m)*(bkm(km)*cos(akm)-gkm(km)*sin(akm)));
     */
    if(barK->GetType() == GENERATION) {
      double generate_power = -barK->GetBSh() * pow(barK->GetVoltage(), 2) + barK->GetRPowerL();
      Quantity * qt = new Quantity();

      qt->SetAttr(PG, barK->GetAPowerG());
      qt->SetAttr(PC, barK->GetAPowerL());
      qt->SetAttr(QC, barK->GetRPowerL());

      container::map<int, Bar*> neighbors = barK->GetNs();
      for(container::map<int, Bar*>::iterator itN = neighbors.begin(); itN != neighbors.end(); itN++) {
        barM = itN->second;
        double theta = barK->GetAngle() - barM->GetAngle();
        Node* edge = barK->GetEdge(barM->GetId());

        double vK = barK->GetVoltage();
        double vM = barM->GetVoltage();

        generate_power += (-(edge->GetS() * pow(1/edge->GetTap(), 2) + edge->GetSh()) * pow(vK, 2) +
                          (vK * 1/edge->GetTap()) * vM * (edge->GetS() * cos(theta) - edge->GetC() * sin(theta)));

        //this->insertLoss(edge, vK, vM, theta);
        this->insertLoss(edge, barK, barM);
      }

      qt->SetAttr(QG, generate_power);
      report->Insert(barK, qt);
    }

    if(barK->GetType() == LOAD) {
      Quantity * qt = new Quantity();

      qt->SetAttr(PG, barK->GetAPowerG());
      qt->SetAttr(QG, barK->GetRPowerG());
      qt->SetAttr(PC, barK->GetAPowerL());
      qt->SetAttr(QC, barK->GetRPowerL());

      report->Insert(barK, qt);

      container::map<int, Bar*> neighbors = barK->GetNs();
      for(container::map<int, Bar*>::iterator itN = neighbors.begin(); itN != neighbors.end(); itN++) {
        barM = itN->second;
        //double theta = barK->GetAngle() - barM->GetAngle();
        Node* edge = barK->GetEdge(barM->GetId());

        /*double vK = barK->GetVoltage();
        double vM = barM->GetVoltage();*/

        //this->insertLoss(edge, vK, vM, theta);
        this->insertLoss(edge, barK, barM);
      }
    }

  }
}

int main(int argc, char ** argv) {
  LoadFlow *lf = new LoadFlow(0.0001);

  lf->Execute("/home/thiago/workspace/ler.txt");
}
