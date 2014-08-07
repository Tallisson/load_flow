#include "load_flow.h"

using namespace std;

#define ANGLE_INIT 0
#define VOLTAGE_INIT 1
#define ERROR 0.0001
#define S_BASE 100

LoadFlow::LoadFlow(double error):
    numB(0), nPV(0), nPQ(0), cont(0), nLT(0), nTAP_Fixed(0), nTap_VC(0), nTap_MVAR(0), nTAP_PHASE(0),
    error(error), sBase(S_BASE), use_base(true), verbose(true)
{
  bars = new Graph();
}

LoadFlow::LoadFlow(double error, double sBase):
    numB(0), nPV(0), nPQ(0), cont(0), error(error), sBase(sBase), use_base(true), verbose(true)
{
  bars = new Graph();
}

LoadFlow::LoadFlow():
    numB(0), nPV(0), nPQ(0), cont(0), nLT(0), nTAP_Fixed(0), nTap_VC(0), nTap_MVAR(0), nTAP_PHASE(0),
    error(ERROR), use_base(true)
{
  bars = new Graph();
}

LoadFlow::~LoadFlow() {
  delete bars;
}

void LoadFlow::AddBar(Bar* bar) {
  if(use_base) {
    bar->SetAPower(bar->GetAPower() / sBase);
    bar->SetRPower(bar->GetRPower() / sBase);

    bar->SetAPowerG(bar->GetAPowerG() / sBase);
    bar->SetAPowerL(bar->GetAPowerL() / sBase);
    bar->SetRPowerG(bar->GetRPowerG() / sBase);
    bar->SetRPowerL(bar->GetRPowerL() / sBase);
    bar->SetBSh(bar->GetBSh() / sBase);
  }

  //cout << "Bar" << bar->GetId() << " (P, Q, Bsh): " << bar->GetAPower() << ", " << bar->GetRPower() << ", " << bar->GetBSh() << endl;

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
  map<int, Bar*> nodes = bars->GetBars();

  for (std::map<int, Bar*>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
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
  map<int, Bar*> nodes = bars->GetBars();

  for (std::map<int, Bar*>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
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

void LoadFlow::initJ() {
  int s = nPV + (nPQ << 1);
  jacobian = zeros<mat>(s, s);
}

void LoadFlow::updateState() {
  Bar * barK;

  std::map<int, int>::iterator it=ord.begin();
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
  /*int i = 0;

  int s = nPQ + nPV;
  for(int k = 0; k < s; k++) {
    if( error < abs(diffP(k)) ) {
      return true;
    }

    i = k + nPQ;
    if( error < abs(diffP(i)) ) {
      return true;
    }
  }

  return false;*/
  vec t = abs(diffP);
  max_error = max(t);
  return  max_error > error;
}

void LoadFlow::mismatches() {
  double theta;
  map<int, Bar*> nodes = bars->GetBars();

  std::map<int, int>::iterator it=ord.begin();
  for(int k = 0; it != ord.end(); k++) {
    Bar * barK = bars->at(it->first);
    Bar * barM;

    if(barK->GetType() != SLACK) {
      double vK = barK->GetVoltage();
      estP(k) = barK->GetC() * pow(vK, 2);
      map<int, Node*> neighbors = barK->GetWeight();

      Node* edge;
      int m;
      for (std::map<int, Node*>::iterator it=neighbors.begin(); it!=neighbors.end(); ++it) {
        m = it->first;
        barM = bars->at(m);
        edge = it->second;
        theta = barK->GetAngle() - barM->GetAngle();

        switch(edge->GetType()) {
        case FIXED_TAP:
        case VARIABLE_TAP_MVAR:
        case VARIABLE_TAP_VC:
          // No lugar de Vk aparece (1/tap * vK)
          estP(k) *= pow(1/edge->GetTap(), 2);
          vK *= 1 / (edge->GetTap());
          break;
        case VARIABLE_PHASE_ANGLE:
          // No lugar de theta aparece (theta + phi)
          theta += edge->GetPhi();
          break;
        default:
          break;
        }

        estP(k) +=  -vK * barM->GetVoltage() * (edge->GetC() * cos(theta) + edge->GetS() * sin(theta));
      }

      diffP(k) = barK->GetAPower() - estP(k);
    }

    if(barK->GetType() == LOAD) {
      int i = ordPQ.at(barK->GetId()) + (nPQ + nPV);
      double vK = barK->GetVoltage();
      estP(i) = -pow(vK, 2) * barK->GetS();
      map<int, Node*> neighbors = barK->GetWeight();

      Node* edge;
      int m;
      for (std::map<int, Node*>::iterator it=neighbors.begin(); it!=neighbors.end(); ++it) {
        m = it->first;
        barM = bars->at(m);
        edge = it->second;
        theta = barK->GetAngle() - barM->GetAngle();

        switch(edge->GetType()) {
        case FIXED_TAP:
        case VARIABLE_TAP_MVAR:
        case VARIABLE_TAP_VC:
          // No lugar de Vk aparece (1/tap * vK)
          estP(i) *= pow(1/edge->GetTap(), 2);
          vK *= 1/ (edge->GetType());
          break;
        case VARIABLE_PHASE_ANGLE:
          // No lugar de theta aparece (theta + phi)
          theta += edge->GetPhi();
          break;
        default:
          break;
        }

        estP(i) += vK * barM->GetVoltage() * (edge->GetS() * cos(theta) - edge->GetC() * sin(theta));
      }

      // Incluindo Susceptância Nodal na Potência reativa
      // Qg(k) + Bsh(k)*V(k)^2 - Qc(k)
      diffP(i) = (barK->GetRPower() + barK->GetBSh() * pow(barK->GetVoltage(), 2)) - estP(i) ;
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

  std::map<int, int>::iterator it=ord.begin();
  int noSlack = nPQ + nPV;
  for(int k = 0; k < noSlack; k++) {
    barK = bars->at(it->first);
    int m = 0;

    if(barK->GetType() != SLACK) {
      map<int, Bar*> neighbors = barK->GetNs();
      for(std::map<int, Bar*>::iterator itN=neighbors.begin(); itN!=neighbors.end(); ++itN) {
        barM = itN->second;
        double theta = barK->GetAngle() - barM->GetAngle();
        Node * edge = barK->GetEdge(barM->GetId());

        // dPk em relação a 'ak '.
        // -V(k)*V(m)*(g(km)*sin(akm)-b(km)*cos(akm)) + Jac(k-1, k-1);
        jacobian(k, k) = -barK->GetVoltage() * barM->GetVoltage() * (edge->GetC() * sin(theta) - edge->GetS() * cos(theta)) + jacobian(k, k);

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
          jacobian(k, m) = -2 * edge->GetC() * barK->GetVoltage() + barM->GetVoltage() * (edge->GetC() * cos(theta) + edge->GetS() * sin(theta))
                                  + jacobian(k, m);
        }

        // dPk em relação a 'vm'.
        if(barM->GetType() == LOAD) {
          // V(k)*(g(km)*cos(akm)+b(km)*sin(akm));
          m = noSlack + ordPQ.at(barM->GetId());
          jacobian(k, m) = barK->GetVoltage() * (edge->GetC() * cos(theta) + edge->GetS() * sin(theta));
        }
      }
    }

    it++;
  }
}

void LoadFlow::DqDer() {
  Bar * barK, * barM;

  std::map<int, int>::iterator it=ordPQ.begin();
  int s = nPV+ (nPQ << 1);
  int noSlack = nPQ + nPV;
  for(int k = nPQ + nPV; k < s; ) {
    barK = bars->at(it->first);
    int m = 0;

    if(barK->GetType() == LOAD) {
      map<int, Bar*> neighbors = barK->GetNs();
      //std::map<int, int>::iterator it = ord.find(barK->GetId());

      for(std::map<int, Bar*>::iterator itN=neighbors.begin(); itN!=neighbors.end(); ++itN) {
        barM = itN->second;
        double theta = barK->GetAngle() - barM->GetAngle();
        Node * edge = barK->GetEdge(barM->GetId());

        // dQk em relaçao a 'ak'.
        // V(k)*V(m)*(b(km)*sin(akm)+g(km)*cos(akm)) + Jac(nb-1+ordPQ(k), k-1);
        m = ord.at(barK->GetId());
        jacobian(k, m) = barK->GetVoltage() * barM->GetVoltage() * (edge->GetS() * sin(theta) + edge->GetC() * cos(theta))
                          + jacobian(k, m);

        // dQk em relaçao a 'am' (exceto quando m for a barra slack).
        if(barM->GetType() != SLACK) {
          // -V(k)*V(m)*(g(km)*cos(akm)+b(km)*sin(akm));
          m = ord.at(barM->GetId());
          jacobian(k, m) = -barK->GetVoltage() * barM->GetVoltage() * (edge->GetC() * cos(theta) + edge->GetS() * sin(theta));
        }

        // dQk em relaçao a 'vk'.
        // 2*(b(km)+bsh(km))*V(k) - V(m)*(b(km)*cos(akm)-g(km)*sin(akm)) + Jac(nb-1+ordPQ(k), nb-1+ordPQ(k));
        jacobian(k, k) = 2 * (edge->GetS()+edge->GetSh()) * barK->GetVoltage() -
                              barM->GetVoltage() * (edge->GetS() * cos(theta) - edge->GetC() * sin(theta)) + jacobian(k, k);
        // dQk em relacao a 'vm'.
        if(barM->GetType() == LOAD) {
          // -V(k)*(b(km)*cos(akm)-g(km)*sin(akm));
          /*m = ord.at(barM->GetId());
          int index = m + nPQ;
          if(index < nPV + nPQ) {
            index++;
          }*/
          m = ordPQ.at(barM->GetId()) + noSlack;
          jacobian(k, m) = -barK->GetVoltage() * (edge->GetS() * cos(theta) - edge->GetC() * sin(theta));
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
  map<int, Bar*> nodes = bars->GetBars();

  for(std::map<int, Bar*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
    barK = it->second;

    if(barK->GetType() == SLACK) {
      double sum = 0;
      map<int, Bar*> neighbors = barK->GetNs();

      for(std::map<int, Bar*>::iterator itN = neighbors.begin(); itN != neighbors.end(); itN++) {
        barM = itN->second;
        double theta = barK->GetAngle() - barM->GetAngle();
        Node* edge = barK->GetEdge(barM->GetId());

        // (gkm(km)*V(k)^2 - V(k)*V(m)*(gkm(km)*cos(akm)+bkm(km)*sin(akm)));
        double vK = barK->GetVoltage();
        sum += (edge->GetC() * pow(vK, 2) - vK * barM->GetVoltage() * (edge->GetC() * cos(theta) + edge->GetS() * sin(theta)) );
      }
      barK->SetAPower(sum);

    }

    if(barK->GetType() != LOAD) {
      double sum = 0;
      map<int, Bar*> neighbors = barK->GetNs();

      for(std::map<int, Bar*>::iterator itN = neighbors.begin(); itN != neighbors.end(); itN++) {
        barM = itN->second;
        double theta = barK->GetAngle() - barM->GetAngle();
        Node* edge = barK->GetEdge(barM->GetId());

        // (-(bkm(km)+bkm_sh(km))*V(k)^2 + V(k)*V(m)*(bkm(km)*cos(akm)-gkm(km)*sin(akm)));
        double vK = barK->GetVoltage();
        sum += (-(edge->GetS() + edge->GetSh()) * pow(vK,2) + vK * barM->GetVoltage() * (edge->GetS() * cos(theta) - edge->GetC() * sin(theta)) );
      }
      barK->SetRPower(sum);

    }
  }
}

int LoadFlow::Execute() {
  int counter = 0;
  initialize();
  mismatches();

  if(verbose == true) {
    /*for(int i = 0; i < numB; i++) {
      Bar * tmp = bars->at(i+1);
      cout << "Bar(" << tmp->GetId() << ")=> (v, a): " << tmp->GetVoltage()  << ", "  << tmp->GetAngle()<< endl;
    }*/

    //cout << "Erro: " << endl << diffP << endl;
  }

  while(true) {
    calcJ();
    solveSys();
    updateState();
    counter++;
    mismatches();

    if(verbose == true) {
      //cout << "Jac" << endl << jacobian << endl;

      //cout << "dx" << endl << diffS << endl;

      /*for(int i = 0; i < numB; i++) {
        Bar * tmp = bars->at(i+1);
        cout << "Bar(" << tmp->GetId() << ")=> (v, a): " << tmp->GetVoltage()  << ", "  << tmp->GetAngle()<< endl;
      }*/

      //cout << "Erro: " << endl << diffP << endl;
    }

    if(nextIterate() == false) {
      break;
    }
  }
  calcS2();
  if(verbose == true) {
    //cout << "O processo convergiu com " << counter << " iterações\n" << endl;

    /*for(int i = 0; i < numB; i++) {
      Bar * b = bars->at(i+1);

      //cout << "Barra(" << b->GetId()+1 << "): Estado(a, v, p, q): (" << b->GetAngle() << ", " << b->GetVoltage() << ", " << b->GetAPower() << ", " << b->GetRPower() << ")" << endl;
      printf("Barra(%d)=> Angulo: %.5f, Voltagem: %.5f, Potência Ativa: %.4f, Potência Reativa: %.4f\n", b->GetId(), b->GetAngle(), b->GetVoltage(), b->GetAPower(), b->GetRPower());
    }*/
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
 */
void LoadFlow::setControlVariables() {
  // deltaU = alfa * deltaZ
  // deltaZ = z_esp - z_cal

}
