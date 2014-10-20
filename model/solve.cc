#include "solve.h"
#include <iostream>

using namespace std;

namespace load
{

Solve::Solve(int cols, int rows)
{
  jacobian = new mat(cols, rows);
}

Solve::~Solve()
{
  jacobian->reset();
  delete jacobian;
  jacobian = NULL;
}

void Solve::FillR()
{
  jacobian->randu(jacobian->n_rows, jacobian->n_rows);
  cout << *(jacobian) << endl;
}

void Solve::SetValue(int col, int row, double value)
{
  jacobian->at(col, row) = value;
}

double Solve::GetValue(int col, int row)
{
  return jacobian->at(col, row);
}

mat* Solve::Product(vec error)
{
  cout << (inv(*jacobian)) << endl;
  mat* m = new mat((inv(*jacobian) * -error));

  return m;
}
}

using namespace load;

int main(int argc, char ** argv)
{
  Solve* s = new Solve(22, 22);
  s->FillR();
  vec v = zeros<vec>(22);
  mat* a = s->Product(v);
  cout << *(a) << endl;

  delete s;

  s = new Solve(22, 22);
  s->FillR();

  return 0;
}
