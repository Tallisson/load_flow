#ifndef REPORT_H_
#define REPORT_H_

#include <boost/container/map.hpp>
#include "bar.h"
#include "quantity.h"
#include "loss.h"

using namespace boost;

namespace load
{

class Report{
private:
  container::map<Bar*, Quantity*> power;
  container::map<Node*, Loss*> losses;
public:
  void Insert(Bar* bar, Quantity* q);
  void Insert(Node* n, Loss* l);
  Quantity* At(Bar* bar);
  Loss* At(Node* n);
  container::map<Bar*, Quantity*> GetPower();
  container::map<Node*, Loss*> GetLosses();

  Report();
  ~Report();
};
}

#endif /* REPORT_H_ */
