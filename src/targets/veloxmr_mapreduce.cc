
#include <../client/vmr.hh>
#include <utility>
#include <string>
#include <sstream>
#include <iostream>

using namespace velox;
using namespace std;

int main(int argc, char** argv) {
  if (argc < 4) {
    cout << "ERROR: usage __velox_mapreduce inputfile mapfunc reducefunc outputfile" << endl;
    return EXIT_FAILURE;
  }

  string map_func = argv[2];
  std::replace(map_func.begin(), map_func.end(), '|', ' ');
  std::replace(map_func.begin(), map_func.end(), '~', '\n');

  string red_func = argv[3];
  std::replace(red_func.begin(), red_func.end(), '|', ' ');
  std::replace(red_func.begin(), red_func.end(), '~', '\n');

  vdfs cloud;
  vmr mr(&cloud);

  dataset A = mr.make_dataset({argv[1]});

  A.pymap(map_func);
  A.pyreduce(red_func, argv[4]);

  return EXIT_SUCCESS;
}
