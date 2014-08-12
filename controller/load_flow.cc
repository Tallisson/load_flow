#include "load_flow.h"

using namespace std;

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
      estP(k) = barK->GetC() * pow(vK, 2);
      container::map<int, Node*> neighbors = barK->GetWeight();

      Node* edge;
      int m;
      for (container::map<int, Node*>::iterator it=neighbors.begin(); it!=neighbors.end(); ++it) {
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
      container::map<int, Node*> neighbors = barK->GetWeight();

      Node* edge;
      int m;
      for (container::map<int, Node*>::iterator it=neighbors.begin(); it!=neighbors.end(); ++it) {
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
          vK *= 1 / (edge->GetTap());
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
        sum += (edge->GetC() * pow(vK, 2) - vK * barM->GetVoltage() * (edge->GetC() * cos(theta) + edge->GetS() * sin(theta)) );
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

  CalcPower();
  if(verbose == true) {
    container::map<Bar*, Quantity*> data = report->GetPower();
    cout << endl << "Grandezas Calculdas: " << endl;

    for(container::map<Bar*, Quantity*>::iterator it = data.begin(); it != data.end(); it++) {
      Bar* barK = it->first;
      Quantity* qt = it->second;

      cout << "Barra " << barK->GetId() << ": { PG [p.u.] = " << qt->GetAttr(PG) << ", QG [p.u.] = " << qt->GetAttr(QG)
           << ", PC [p.u.] = " << qt->GetAttr(PC) <<  ", QC [p.u.] = " << qt->GetAttr(QC) << " }" << endl;
    }
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
    Bar* crt_bar = bars->at(edge->GetBar());

    // deltaU = alfa * deltaZ
    // deltaZ = z_esp - z_cal
    switch(edge->GetType()) {
    case FIXED_TAP:

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

void LoadFlow::CalcPower() {
  Bar * barK, * barM;
  container::map<int, Bar*> nodes = bars->GetBars();

  for(container::map<int, Bar*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
    barK = it->second;

    /*
     * Para todas as barras do tipo slack (tipo 3):
     * 1) Pg(k) = Pc(k)
     * 2) Pg(k) = Pg(k) + G(km) * (V(k) ^ 2 - V(k) * V(m) *(G(km) * cos(theta) + Bkm * sen(theta)))     *
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
        double theta = barK->GetAngle() - barM->GetAngle();
        Node* edge = barK->GetEdge(barM->GetId());

        // (gkm(km)*V(k)^2 - V(k)*V(m)*(gkm(km)*cos(akm)+bkm(km)*sin(akm)));
        double vK = barK->GetVoltage();

        generate_power += (edge->GetC() * pow(vK, 2) - vK * barM->GetVoltage() *
                          (edge->GetC() * cos(theta) + edge->GetS() * sin(theta)) );
        reactive_power += (-(edge->GetS() + edge->GetSh()) * pow(vK, 2) +
                          vK * barM->GetVoltage() * (edge->GetS() * cos(theta) - edge->GetC() * sin(theta)));
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

        generate_power += (-(edge->GetS() + edge->GetSh()) * pow(vK, 2) +
                              vK * barM->GetVoltage() * (edge->GetS() * cos(theta) - edge->GetC() * sin(theta)));
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
    }
  }
}

int main(int argc, char ** argv) {
  LoadFlow * lf = new LoadFlow(0.0001);

  Bar * b1 = new Bar(0, 1.060, 0.0, 0.0, 232.40, -16.90, SLACK, 1, 0);
  Bar * b2 = new Bar(0, 1.045, 21.7, 12.7, 40.00, 42.4, GENERATION, 2, 0);
  Bar * b3 = new Bar(0, 1.010, 94.2, 19.0, 0, 23.4, GENERATION, 3, 0);
  Bar * b4 = new Bar(0,     0, 47.8, -3.9, 0, 0, LOAD, 4, 0);
  Bar * b5 = new Bar(0,     0, 7.6, 1.6, 0, 0, LOAD, 5, 0);
  Bar * b6 = new Bar(0, 1.070, 11.2, 7.5, 0, 12.2, GENERATION, 6, 0);
  Bar * b7 = new Bar(0,     0, 0, 0, 0, 0, LOAD, 7, 0);
  Bar * b8 = new Bar(0, 1.090, 0, 0, 0, 17.4, GENERATION, 8, 0);
  Bar * b9 = new Bar(0,     0, 29.5, 16.6, 0, 0, LOAD, 9, 0.19);
  Bar * b10 = new Bar(0,    0, 9, 5.8, 0, 0, LOAD, 10, 0);
  Bar * b11 = new Bar(0,    0, 3.5, 1.8, 0, 0, LOAD, 11, 0);
  Bar * b12 = new Bar(0,    0, 6.1, 1.6, 0, 0, LOAD, 12, 0);
  Bar * b13 = new Bar(0,    0, 13.5, 5.8, 0, 0, LOAD, 13, 0);
  Bar * b14 = new Bar(0,    0, 14.9, 5.0, 0, 0, LOAD, 14, 0);

  lf->AddBar(b1);
  lf->AddBar(b2);
  lf->AddBar(b3);
  lf->AddBar(b4);
  lf->AddBar(b5);
  lf->AddBar(b6);
  lf->AddBar(b7);
  lf->AddBar(b8);
  lf->AddBar(b9);
  lf->AddBar(b10);
  lf->AddBar(b11);
  lf->AddBar(b12);
  lf->AddBar(b13);
  lf->AddBar(b14);

  //rkm    = [0.01938  0.05403  0.04699  0.05811  0.05695  0.06701  0.01335   0.0      0.0      0.0     0.09498   0.12291  0.06615  0.0     0.0      0.03181  0.12711  0.08205  0.22092  0.17093];
  //xkm    = [0.05917  0.22304  0.19797  0.17632  0.17388  0.17103  0.04211  0.20912  0.55618  0.25202  0.19890  0.25581  0.13027  0.17615  0.11001  0.08450  0.27038  0.19207  0.19988  0.34802]';
  //bkm_sh = [0.0528   0.0492   0.0438   0.0340   0.0346   0.0128   0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0      0.0]'/2;

  Admitt * a1 = new Admitt(0.01938, 0.05917, 0.0528/2);
  Admitt * a2 = new Admitt(0.05403, 0.22304, 0.0492/2);
  Admitt * a3 = new Admitt(0.04699, 0.19797, 0.0438/2);
  Admitt * a4 = new Admitt(0.05811, 0.17632, 0.0340/2);
  Admitt * a5 = new Admitt(0.05695, 0.17388, 0.0346/2);
  Admitt * a6 = new Admitt(0.06701, 0.17103, 0.0128/2);
  Admitt * a7 = new Admitt(0.01335, 0.04211, 0);
  Admitt * a8 = new Admitt(0, 0.20912, 0);
  Admitt * a9 = new Admitt(0, 0.55618, 0);
  Admitt * a10 = new Admitt(0, 0.25202, 0);
  Admitt * a11 = new Admitt(0.09498, 0.19890, 0);
  Admitt * a12 = new Admitt(0.12291, 0.25581, 0);
  Admitt * a13 = new Admitt(0.06615, 0.13027, 0);
  Admitt * a14 = new Admitt(0, 0.17615, 0);
  Admitt * a15 = new Admitt(0, 0.11001, 0);
  Admitt * a16 = new Admitt(0.03181, 0.08450, 0);
  Admitt * a17 = new Admitt(0.12711, 0.27038, 0);
  Admitt * a18 = new Admitt(0.08205, 0.19207, 0);
  Admitt * a19 = new Admitt(0.22092, 0.19988, 0);
  Admitt * a20 = new Admitt(0.17093, 0.34802, 0);

  lf->AssocBars(b1, b2, a1);
  lf->AssocBars(b1, b5, a2);
  lf->AssocBars(b2, b3, a3);
  lf->AssocBars(b2, b4, a4);
  lf->AssocBars(b2, b5, a5);
  lf->AssocBars(b3, b4, a6);
  lf->AssocBars(b4, b5, a7);
  lf->AssocBars(b4, b7, a8);
  lf->AssocBars(b4, b9, a9);
  lf->AssocBars(b5, b6, a10);
  lf->AssocBars(b6, b11, a11);
  lf->AssocBars(b6, b12, a12);
  lf->AssocBars(b6, b13, a13);
  lf->AssocBars(b7, b8, a14);
  lf->AssocBars(b7, b9, a15);
  lf->AssocBars(b9, b10, a16);
  lf->AssocBars(b9, b14, a17);
  lf->AssocBars(b10, b11, a18);
  lf->AssocBars(b12, b13, a19);
  lf->AssocBars(b13, b14, a20);

  lf->Execute();
  delete lf;
}
