#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <boost/cstdint.hpp>
#include <boost/regex.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/container/vector.hpp>
#include "bar.h"
#include "admitt.h"
#include "define.h"


typedef struct Desc {
public:
  std::string name_system;
  double s_base;
  container::vector<Bar*> bars;
  container::vector<Admitt*> admmits;
  Desc(): name_system(""), s_base(0) {};
} Desc;

class Utils{
private:
  Utils() {};
  Utils(Utils const&){};             // copy constructor is private
  ~Utils() {};

  static Utils* instance;
public:
  static Utils* GetInstance();
  Desc* ProcessFile(const char* file);
  void Delete();
  bool ValidateInfSystem(const std::string& s);
  bool ValidateBusData(const std::string& s);
  bool ValidateBranchData(const std::string& s);
};


#endif /* UTILS_H_ */
