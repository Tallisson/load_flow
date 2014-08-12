#ifndef LOSS_H_
#define LOSS_H_

#include "define.h"

class Loss{
private:
  double p_in_branch;
  double q_in_branch;
  double loss;
public:
  Loss();
  ~Loss();
  void SetAttr(int attr, double value);
  double GetAttr(int attr);
};


#endif /* LOSS_H_ */
