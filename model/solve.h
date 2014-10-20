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

public:
  Solve(int cols, int rows);
  ~Solve();

  void FillR();
  void SetValue(int col, int row, double value);
  double GetValue(int col, int row);
  mat* Product(vec error);

};
}

#endif /* SOLVE_H_ */
