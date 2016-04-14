#include <mapreduce/nodes/remotemr.h>
#include <common/context.hh>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {
  RemoteMR nl;
  nl.establish();

  return context.join();
}
