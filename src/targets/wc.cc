#include "../mapreduce/dataset.hh"

#include <utility>
#include <string>
#include <sstream>

using namespace eclipse;
using namespace std;

extern "C" {
  pair<string, string> mymapper(string);
  pair<string, string> mymapper2(string);
  string myreducer (string, string);
}

pair<string, string> mymapper(string line) {

  std::stringstream  stream(line);
  std::string        oneWord;
  unsigned int       count = 0;

  while(stream >> oneWord) { ++count;}
  
  return {"Total", to_string(count)};
}

pair<string, string> mymapper2(string line) {

  std::stringstream  stream(line);
  std::string        oneWord;
  unsigned int       count = 0;

  while(stream >> oneWord) { ++count;}
  
  return {"Total1", to_string(count)};
}

string myreducer (string a, string b) {
  return to_string(stoi(a) + stoi(b));
}

int main (int argc, char** argv) {
  DataSet& A = DataSet::open(argv[1]);
  A.map("mymapper");
  A.reduce("myreducer", "iteration_1");

  DataSet& B = DataSet::open("iteration_1");
  B.map("mymapper2");
  B.reduce("myreducer", "output");
}
