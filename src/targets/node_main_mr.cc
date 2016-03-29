#include <mapreduce/remotemr.hh>
#include <common/context.hh>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {
  Context context;
  context.run();

  RemoteMR nl (context);
  nl.establish();

  return context.join();
}
