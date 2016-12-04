#include <eclipsedfs/vmr.hh>
#include <utility>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;
using namespace velox;

extern "C" {
  void mymapper(std::string, velox::MapOutputCollection&);
  string myreducer (string, string);
}

string myreducer (string a, string b) {
  return to_string(stoi(a) + stoi(b));
}

void mymapper(std::string line, velox::MapOutputCollection& mapper_results) {
  std::stringstream  stream(line);
  std::string token;

  while(stream >> token) 
    mapper_results.insert(token, to_string(1));
}

int main (int argc, char** argv) {
  vdfs cloud;
  file myfile = cloud.open("BUM");
  myfile.append(
      "HELLO HOLA HELLO\n"
      "HELLO BYE\n"
      "BYE\n"
      "BYE\n"
      "HOLA MCCREE\n"
      "HOLA ANA\n"
      "BYE ZZ\n"
      "HOLA\n"
      "BYE\n"
      "ADIOS\n");
  cout << myfile.get() << endl;
  vmr mr(&cloud);
  dataset A = mr.make_dataset({"BUM"});
  A.map("mymapper");
  A.reduce("myreducer", "wc.result");
  cloud.rm("BUM");

  file result_file = cloud.open("wc.result");
  cout << result_file.get() << endl;
  
  return 0;
}
