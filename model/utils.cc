#include "utils.h"

using namespace boost;

Utils * Utils::instance = NULL;

Utils* Utils::GetInstance() {
  if(!instance) {
    instance = new Utils;
  }

  return instance;
}

void Utils::Delete() {
  delete instance;
}

bool Utils::ProcessFile(const char* filename) {
  Desc d;
  std::ifstream file(filename);
  boost::iostreams::filtering_istream in;
  in.push(file);
  int qtd_bus = 0;
  int qtd_branch = 0;

  for(std::string str; std::getline(in, str); )
  {
      if((qtd_bus == 0 && qtd_branch == 0) && this->ValidateInfSystem(str)) {
        container::vector<std::string> strs;
        split(strs, str, is_any_of("\t "));
        if(strs.size() == 3) {
          d.name_system = strs.at(1);
          d.s_base = atof(strs.at(2).c_str());
        }
      }

      if(qtd_bus > 0) {
        container::vector<std::string> strs;
        split(strs, str, is_any_of("\t "));
        unsigned size = strs.size();
        if(size == 9) {
          Bar * b = new Bar(strs);
          d.bars.push_back(b);
        }
        qtd_bus--;
      }

      if((qtd_bus == 0 && qtd_branch == 0) && this->ValidateBusData(str)) {
        container::vector<std::string> strs;
        split(strs, str, is_any_of("\t "));
        if(strs.size() == 3) {
          qtd_bus = atoi(strs.at(2).c_str());
        }
      }

      if(qtd_branch > 0) {
        container::vector<std::string> strs;
        split(strs, str, is_any_of("\t "));
        unsigned size = strs.size();
        if(size == 8) {
          Admitt * ad = new Admitt(strs);
          d.admmits.push_back(ad);
        }
        qtd_branch--;
      }

      if((qtd_bus == 0 && qtd_branch == 0) && this->ValidateBranchData(str)) {
        container::vector<std::string> strs;
        split(strs, str, is_any_of("\t "));
        if(strs.size() == 3) {
          qtd_branch = atoi(strs.at(2).c_str());
        }
      }

      //std::cout << str << '\n';
  }

  cout << "System " << d.name_system << endl;
  cout << "Base " << d.s_base << endl;
  cout << "Numero de barras: " << d.bars.size() << endl;
  cout << "Numero de trafos : " << d.admmits.size() << endl;
  return true;
}

bool Utils::ValidateInfSystem(const std::string& s) {
  regex e("\\A(System[\\s]{1}[\\S]+[\\s]{1}[\\d]{2, 4})\\z");
  return regex_match(s, e);
}

bool Utils::ValidateBusData(const std::string& s) {
  regex e("\\A(Bus Data[\\s]{1}[\\d]{1, 3})\\z");
  return regex_match(s, e);
}

bool Utils::ValidateBranchData(const std::string& s) {
  regex e("\\A(Branch Data[\\s]{1}[\\d]{1, 3})\\z");
  return regex_match(s, e);
}

int main(int argc, char ** argv)
{
  Utils* util = Utils::GetInstance();
  util->ProcessFile("/home/thiago/workspace/ler.txt");
  util->Delete();

  /*std::string input_stlstr = "System 14-bus 100";
  std::cout << util->ValidateInfSystem(input_stlstr) << std::endl;

  std::string input_t = "Bus Data 14";
  std::cout << util->ValidateBusData(input_t) << std::endl;
  std::string input_s = "Branch Data A";
  std::cout << util->ValidateBranchData(input_s) << std::endl;*/

  return 0;
}
