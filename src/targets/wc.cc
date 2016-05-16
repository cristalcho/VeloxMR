#include "../mapreduce/dataset.hh"

#include <utility>
#include <string>

using namespace eclipse;
using namespace std;

extern "C" {
  pair<string, string> myfunc (string);
  string myreducer (string, string);
}

pair<string, string> myfunc (string a) {

  int total = 0;
  char *p = new char[a.length()];
  strncpy (p, a.c_str(), a.length());
  p = strtok(p, " ");
  while (p) {
    if (p[0] != '\n' or strlen(p) == 0)
    total++;
    p = strtok(NULL, " ");
  }

  delete p;
  
  auto output = to_string(total);
  return {"Total", output};
}

string myreducer (string a, string b) {
  auto a_ = atoi (a.c_str());
  auto b_ = atoi (b.c_str());

  auto out = to_string(a_ + b_);
  
  return out;
}

int main (int argc, char** argv) {
  DataSet& A = DataSet::open(argv[1]);
  A.map("myfunc");
  A.reduce("myreducer");
}
