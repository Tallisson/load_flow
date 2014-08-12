#ifndef LOSS_H_
#define LOSS_H_

#define P_IN 40
#define Q_IN 50

class Loss{
private:
  double p_in_branch;
  double q_in_branch;
public:
  Loss();
  ~Loss();
  void SetAttr(int attr, double value);
  double GetAttr(int attr);
};


#endif /* LOSS_H_ */
