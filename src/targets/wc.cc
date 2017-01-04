#include <eclipsedfs/vmr.hh>
#include <eclipsedfs/output_collection.hh>
#include <utility>
#include <string>
#include <sstream>
#include <iostream>
#include <list>
#include <unordered_map>

using namespace std;
using namespace velox;

extern "C" {
  void mymapper(std::string, velox::OutputCollection&, std::unordered_map<std::string, void*>&);
  void myreducer(std::string, std::list<std::string>, velox::OutputCollection&);
}

void mymapper(std::string line, velox::OutputCollection& mapper_results, std::unordered_map<std::string, void*>& options) {
  std::stringstream  stream(line);
  std::string token;

  while(stream >> token) 
    mapper_results.insert(token, to_string(1));
}

void myreducer(std::string key, std::list<std::string> values, velox::OutputCollection& output) {
  int sum = 0;
  for(std::string& value : values) 
    sum += stoi(value); 
  output.insert(key, to_string(sum));
}

int main (int argc, char** argv) {
  const std::string input_name = "1G_input.txt";
  const std::string output_name = "wc.result";

  vdfs cloud;
  file myfile = cloud.open(input_name);

  vmr mr(&cloud);
  dataset A = mr.make_dataset({input_name});

  A.map("mymapper");
  A.reduce("myreducer", output_name);

  return 0;
}
