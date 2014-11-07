#include "solve.h"
#include <iostream>

using namespace std;

namespace load
{

Solve::Solve(int rows, int cols):
  rows(rows), cols(cols)
{
  data = new mat(cols, rows);
}

Solve::~Solve()
{
}

void Solve::FillR()
{
  data->randu(data->n_rows, data->n_rows);
  cout << *(data) << endl;
}

void Solve::SetValue(int row, int col, double value)
{
  (data->at(row, col)) = value;
}

void Solve::Zeros()
{
  data->zeros(rows, cols);
}

void Solve::Clear()
{
  data->reset();
  delete data;
}

void Solve::Print()
{
  cout << *(data) << endl;
}

double Solve::GetValue(int row, int col)
{
  return data->at(row, col);
}

mat Solve::Product(vec error)
{
  mat m = mat(((inv(*data)) * -error));

  return m;
}

mat Solve::Inverse()
{
  return inv(*(data));
}
}
