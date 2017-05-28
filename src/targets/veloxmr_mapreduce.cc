
#include <../client/vmr.hh>
#include <utility>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace velox;
using namespace std;

int main(int argc, char** argv) {
  if (argc < 6) {
    cout << "ERROR: usage __velox_mapreduce inputfile mapfunc reducefunc outputfile [premap] [aftermap]" << endl;
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

  string pmap_src = "";
  string amap_src = "";
  string pmap_func = argv[5];

  if (pmap_func != "NULL") {
    std::replace(pmap_func.begin(), pmap_func.end(), '|', ' ');
    std::replace(pmap_func.begin(), pmap_func.end(), '~', '\n');
    pmap_src = pmap_func;
  }

  string amap_func = argv[6];

  if (amap_func != "NULL") {
    std::replace(amap_func.begin(), amap_func.end(), '|', ' ');
    std::replace(amap_func.begin(), amap_func.end(), '~', '\n');
    amap_src = amap_func;
  }

  A.pymap(map_func, pmap_src, amap_src);

  A.pyreduce(red_func, argv[4]);

  return EXIT_SUCCESS;
}
