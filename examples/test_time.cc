#include <iostream>
#include <fstream>
#include <string>
#include <armadillo>
#include "../controller/load_flow.h"
#include "case_2_bus.h"
#include "case_5_bus.h"
#include "case_6_bus.h"
#include "case_14_bus.h"

#define FILE "/home/thiago/Mestrado/paa/simulacoes-artigo/example"
#define ITERACOES 50
#define REPLICACOES 150
#define ERROR_TEST 0.003

using namespace std;
using namespace arma;

double diff(timespec start, timespec end);
void case2();
void case5();
void case6();
void case14();
template <class T>
string to_string(T m);

bool write_file(string t);

/*int main(int argc, char ** argv)
{
  case2();
  //case5();
  case6();
  case14();

  return 0;
}*/

// Devolve tempo em segundos
double diff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return ((temp.tv_sec + (temp.tv_nsec / 1000000000.0))*1000);
}

bool write_file(string t) {
  ofstream myfile;
  myfile.open (FILE, ios::app);
  if (myfile.is_open()) {
      myfile << t << "\n";
      myfile.close();

      return true;
  }

  return false;

}

void case2() {
  vec times = zeros<vec>(ITERACOES);
  timespec time1, time2;

  // Aquecimento
  init_2(ERROR_TEST);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
  for(int i = 0; i < 10000; i++) {
    exec_2();
  }
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
  cout << "Tempo Aquecimento: " << diff(time1, time2) << endl;

  int c;
  for(int i = 0; i < ITERACOES; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    for(int j = 0; j < REPLICACOES; j++) {
      c = exec_2();
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    times(i) = diff(time1, time2);
    cout << "Iteracao " << i << ", tempo: " << times(i) << endl;
  }
  finish_2();

  ostringstream t;
  string media = to_string(mean(times));
  string desvio = to_string(stddev(times));
  string iteracoes = to_string(c);
  t << "(2bus)=>Media: " << media << ", Desvio Padrao: " << desvio << ", Num Iterações: " << iteracoes << "\n";
  write_file(t.str());

  cout << "(2bus)=>Media: " << media << ", Desvio Padrao: " << desvio << ", Num Iterações: " << iteracoes << "\n";}

void case5() {
  vec times = zeros<vec>(ITERACOES);
  timespec time1, time2;

  // Aquecimento
  init_5(ERROR_TEST);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
  for(int i = 0; i < 10000; i++) {
    exec_5();
  }
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
  cout << "Tempo Aquecimento: " << diff(time1, time2) << endl;

  int c;
  for(int i = 0; i < ITERACOES; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    for(int j = 0; j < REPLICACOES; j++) {
      c = exec_5();
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    times(i) = diff(time1, time2);
    cout << "Iteracao " << i << ", tempo: " << times(i) << endl;
  }
  finish_5();

  ostringstream t;
  string media = to_string(mean(times));
  string desvio = to_string(stddev(times));
  string iteracoes = to_string(c);
  t << "(5bus)=>Media: " << media << ", Desvio Padrao: " << desvio << ", Num Iterações: " << iteracoes << "\n";
  write_file(t.str());

  cout << "(5bus)=>Media: " << media << ", Desvio Padrao: " << desvio << ", Num Iterações: " << iteracoes << "\n";
}

void case6() {
  vec times = zeros<vec>(ITERACOES);
  timespec time1, time2;

  // Aquecimento
  init_6(ERROR_TEST);
  int c;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
  for(int i = 0; i < 10000; i++) {
    c = exec_6();
  }
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
  cout << "Tempo Aquecimento: " << diff(time1, time2) << endl;

  for(int i = 0; i < ITERACOES; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    for(int j = 0; j < REPLICACOES; j++) {
      c = exec_6();
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    times(i) = diff(time1, time2);
    cout << "Iteracao " << i << ", tempo: " << times(i) << ", Iteracoes: " << c <<endl;
  }
  finish();

  ostringstream t;
  string media = to_string(mean(times));
  string desvio = to_string(stddev(times));
  string iteracoes = to_string(c);
  t << "(6bus)=>Media: " << media << ", Desvio Padrao: " << desvio << ", Num Iterações: " << iteracoes << "\n";
  write_file(t.str());

  cout << "(6bus)=>Media: " << media << ", Desvio Padrao: " << desvio << ", Num Iterações: " << iteracoes << "\n";
}

void case14() {
  vec times = zeros<vec>(ITERACOES);
  timespec time1, time2;

  // Aquecimento
  init_14(ERROR_TEST);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
  for(int i = 0; i < 10000; i++) {
    exec_14();
  }
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
  cout << "Tempo Aquecimento: " << diff(time1, time2) << endl;

  int c;
  for(int i = 0; i < ITERACOES; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    for(int j = 0; j < REPLICACOES; j++) {
      c = exec_14();
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    times(i) = diff(time1, time2);
    cout << "Iteracao " << i << ", tempo: " << times(i) << endl;
  }
  finish_14();

  ostringstream t;
  string media = to_string(mean(times));
  string desvio = to_string(stddev(times));
  string iteracoes = to_string(c);
  t << "(14bus)=>Media: " << media << ", Desvio Padrao: " << desvio << ", Num Iterações: " << iteracoes << "\n";
  write_file(t.str());

  cout << "(14bus)=>Media: " << media << ", Desvio Padrao: " << desvio << ", Num Iterações: " << iteracoes << "\n";
}

template <class T>
string to_string(T m) {
  ostringstream oss;
  oss << m;
  return oss.str();
}
