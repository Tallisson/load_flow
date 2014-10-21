#ifndef SOLVE_H_
#define SOLVE_H_

#include <armadillo>

using namespace arma;

namespace load
{
class Solve
{
private:
  mat* jacobian;
  int rows;
  int cols;
public:
  Solve(int cols, int rows);
  virtual ~Solve();

  void FillR();
  void SetValue(int col, int row, double value);
  void Zeros();
  void Clear();
  void Print();

  double GetValue(int col, int row);

  mat Product(vec error);
};
}

#endif /* SOLVE_H_ */
