#include "../mapreduce/dataset.hh"

#include <utility>

using namespace eclipse;
using namespace std;

extern "C" {
  pair<string, string> myfunc (string);
}


pair<string, string> myfunc (string a) {
  auto len = a.length();
  auto len_str = to_string(len);
  
  return {"First", len_str };
}
int main (int argc, char** argv) {
  DataSet& A = DataSet::open(argv[1]);
  A.map("myfunc");
}
