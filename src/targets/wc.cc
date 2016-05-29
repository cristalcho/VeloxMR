#include "../mapreduce/dataset.hh"

#include <utility>
#include <string>
#include <sstream>

using namespace eclipse;
using namespace std;

extern "C" {
  pair<string, string> mymapper(string);
  string myreducer (string, string);
}

pair<string, string> mymapper(string a) {

  std::stringstream  stream(a);
  std::string        oneWord;
  unsigned int       count = 0;

  while(stream >> oneWord) { ++count;}
  
  return {"Total", to_string(count)};
}

string myreducer (string a, string b) {
  return to_string(stoi(a) + stoi(b));
}

int main (int argc, char** argv) {
  DataSet& A = DataSet::open(argv[1]);
  A.map("mymapper");
  A.reduce("myreducer");
}
