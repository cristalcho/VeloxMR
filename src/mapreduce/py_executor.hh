//
// @file py_executor.hh
// @author Vicente Adolfo Bolea Sanchez 
// @date December 3rd, 2016
//
// @brief This file abstract the python interpreter for 
// our mapreduce framework
//
// @todo remote the python header include from the header file.
// @todo Merge with c++ executor using a more elegant design pattern.
//
#pragma once
#include "nodes/peermr.h"
#include "../messages/task.hh"

#include <utility>
#include <map>
#include <vector>
#include <python2.7/Python.h>

namespace eclipse {

class PYexecutor {
  public:
    PYexecutor(PeerMR*);
    ~PYexecutor();

    bool run_map(messages::Task*, std::string);
    bool run_reduce(messages::Task*);

  protected:
    PeerMR* peer;

    std::map<std::string, std::vector<std::string>> py_map(std::string);
    std::string py_reduce(std::string, std::string);
    void py_declare_func(std::string);
   
    PyObject* python_module;
};

} /* eclipse  */
