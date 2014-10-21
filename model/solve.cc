#include "solve.h"
#include <iostream>

using namespace std;

namespace load
{

Solve::Solve(int rows, int cols):
  rows(rows), cols(cols)
{
  jacobian = new mat(cols, rows);
}

Solve::~Solve()
{
}

void Solve::FillR()
{
  jacobian->randu(jacobian->n_rows, jacobian->n_rows);
  cout << *(jacobian) << endl;
}

void Solve::SetValue(int row, int col, double value)
{
  (jacobian->at(row, col)) = value;
}

void Solve::Zeros()
{
  jacobian->zeros(rows, cols);
}

void Solve::Clear()
{
  jacobian->reset();
  delete jacobian;
}

void Solve::Print()
{
  cout << *(jacobian) << endl;
}

double Solve::GetValue(int row, int col)
{
  return jacobian->at(row, col);
}

mat Solve::Product(vec error)
{
  mat m = mat(((inv(*jacobian)) * -error));

  return m;
}
}
