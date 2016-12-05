// vim: foldmethod=marker
#include "py_executor.hh"
#include "../common/hash.hh"
#include "../mapreduce/messages/key_value_shuffle.h"
#include "../mapreduce/fs/ireader.h"
#include "../messages/keyvalue.hh"

#include <python2.7/Python.h>
#include <string>
#include <sstream>
#include <utility>
#include <functional>

using namespace eclipse;
using namespace std;

namespace eclipse {
// Constructor {{{
PYexecutor::PYexecutor(PeerMR* p) : peer(p) { }
PYexecutor::~PYexecutor() { }
// }}}
// run_map {{{
bool PYexecutor::run_map (messages::Task* m, std::string input) {
  Py_Initialize();
  py_declare_func(m->func_body);

  stringstream ss (input);

  std::map<string, vector<string>> map_output;
  char next_line[10000]; //! :TODO: change to DFS line limit

  while (!ss.eof()) {
    bzero(next_line, 10000);
    ss.getline (next_line, 10000);
    if (strnlen(next_line, 10000) == 0) 
      continue;

    auto key_value = py_map(string(next_line));

    for (auto& kv_pair : key_value) {
      auto& key   = kv_pair.first;
      auto& value = kv_pair.second;

      if (map_output.find(key) != map_output.end()) {
        std::copy(value.begin(), value.end(), back_inserter(map_output[key]));
      
      } else {
        map_output[key] = vector<string>(value.begin(), value.end());
      }
    }
  }

  for (auto& kv_pair : map_output) {
    KeyValueShuffle kv; 
    kv.job_id_ = m->job_id;           // :TODO:
    kv.map_id_ = 0;
    kv.key_ = kv_pair.first;
    kv.value_ = kv_pair.second; 
    peer->process(&kv);
  }

  Py_XDECREF(python_module);
  Py_Finalize(); 

  return true;
}
// }}}
// run_reduce {{{
bool PYexecutor::run_reduce (messages::Task* task) {
  auto block_size = GET_INT("filesystem.block");
  Py_Initialize();
  py_declare_func(task->func_body);   // Declare the function once to speedup

  try {
    IReader ireader;
    ireader.set_job_id(task->job_id);
    ireader.set_map_id(0);            // :TODO:
    ireader.set_reducer_id(0);
    ireader.init();

    uint32_t iterations = 0;
    uint32_t total_size = 0;
    std::string block_content;
    while (ireader.is_next_key()) {
      string key;
      ireader.get_next_key(key);

      int total_iterations = 0;
      string last_output;
      if (ireader.is_next_value()) {
        ireader.get_next_value(last_output);
        total_iterations = 1;

        while (ireader.is_next_value()) {
          string value;
          ireader.get_next_value(value);

          last_output = py_reduce(value, last_output);

          total_iterations++;
        }
      }
      INFO("Key %s #iterations: %i", key.c_str(), total_iterations);
      if (block_content.length() + last_output.length() > (uint32_t)block_size) {
        BlockInfo bi;
        bi.file_name = task->file_output;
        bi.name = task->file_output + "-" + key.c_str();
        bi.seq = iterations;
        bi.hash_key = h(bi.name);
        bi.size = block_content.length();
        bi.content = block_content;
        bi.replica = 1;
        bi.node = "";
        bi.l_node = "";
        bi.r_node = "";
        bi.is_committed = 1;

        dynamic_cast<PeerMR*>(peer)->submit_block(&bi);
        iterations++;
        total_size += block_content.length();
        block_content = "";

      } else if (!ireader.is_next_key()) {
        block_content += key + ":" + last_output + "\n";
        BlockInfo bi;
        bi.file_name = task->file_output;
        bi.name = task->file_output + "-" + key.c_str();
        bi.seq = iterations;
        bi.hash_key = h(bi.name);
        bi.size = block_content.length();
        bi.content = block_content;
        bi.replica = 1;
        bi.node = "";
        bi.l_node = "";
        bi.r_node = "";
        bi.is_committed = 1;

        dynamic_cast<PeerMR*>(peer)->submit_block(&bi);
        iterations++;
        total_size += block_content.length();
        block_content = "";
      }

      block_content += key + ":" + last_output + "\n";

    }

    FileInfo fi;
    fi.name = task->file_output;
    fi.num_block = iterations;
    fi.size = total_size;
    fi.hash_key = h(fi.name);
    fi.replica = 1;

    dynamic_cast<PeerMR*>(peer)->process(&fi);

  } catch (std::exception& e) {
    PANIC("Error in the executer: %s", e.what());
  }

  Py_XDECREF(python_module);
  Py_Finalize(); 
  return true;
}
// }}}
// py_declare_func {{{
// @brief it compiles and load into the intepreter the indicated function
void PYexecutor::py_declare_func(string func) {
  PyObject *pCompiledFn, *pModule;
  char module_name[] = "VELOXMR_BACKEND";

  pCompiledFn = Py_CompileString(func.c_str(), "", Py_file_input);

  if (pCompiledFn == NULL)
    ERROR("[PY interpreter] I am not able to parse your function");

  pModule = PyImport_ExecCodeModule(module_name, pCompiledFn);

  if (pModule == NULL)
    ERROR("[PY interpreter] I am not able to create a module for your function");

  python_module = pModule;
}
// }}}
// py_map {{{
// @brief it runs the map function in python
//
// The python map function returns a dictionary
// we iterate the dictionary to return all the 
// key pair functions
//
// @todo multiples key values
std::map<string, vector<string>> PYexecutor::py_map(string line) {
  PyObject  *key = NULL, *value = NULL, *pFunc = NULL, *pValue = NULL;
  Py_ssize_t pos = 0;

  pFunc  = PyObject_GetAttrString(python_module, "map" ) ;
  pValue = PyObject_CallFunction(pFunc, "s", const_cast<char*>(line.c_str()));

  if (pValue == NULL) {
    ERROR("Python map did not return anything :( ");
  }

  std::map<string, vector<string>> ret;

  while (PyDict_Next(pValue, &pos, &key, &value)) {
    string k = PyString_AsString(key);

    for (Py_ssize_t i = 0; i < PyList_Size(value); i++) {
      PyObject* item = PyList_GetItem(value, i);
      string v = PyString_AsString(item);

      if (ret.find(k) != ret.end()) {
        ret[k].push_back(v);

      } else {
        ret.insert({k, {v}});
      }
    }
  }

  Py_DECREF(pValue);
  return ret;
}
// }}}
// py_reduce {{{
// @brief it runs the reduce python function
//
// The reduce python function always return a 
// string. 
string PYexecutor::py_reduce(string a, string b) {
  PyObject* pFunc = PyObject_GetAttrString(python_module, "reduce" ) ;
  PyObject* pValue = PyObject_CallFunction(pFunc, "ss", a.c_str(), b.c_str());

  if (pValue == NULL) {
    ERROR("Python reduce function did not return anything");
  }

  string ret = PyString_AsString(pValue);

  Py_DECREF(pValue);

  return ret;
}
// }}}
} /* eclipse  */
