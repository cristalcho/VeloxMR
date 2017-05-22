#include "task_python.hh"
#include "../common/context_singleton.hh"

#include <python2.7/Python.h>
#include <exception>
#include <memory>
#include <utility>

using namespace eclipse;
using namespace std;

// Constructor {{{
task_python::task_python(std::string function_body, 
    std::string pre_map = "", 
    std::string after_map = "") :
  function_body_f(function_body),
  pre_map_f(pre_map),
  after_map_f(after_map)
{ }

task_python::~task_python() {
  Py_XDECREF(python_module);
  Py_Finalize(); 
}
// }}}
// setup {{{
void task_python::setup(bool is_map) {
  Py_Initialize();

  PyObject *pCompiledFn, *pModule;
  char module_name[] = "VELOXMR_BACKEND";

  pCompiledFn = Py_CompileString(function_body_f.c_str(), "", Py_file_input);

  if (pCompiledFn == NULL)
    ERROR("[PY interpreter] I am not able to parse your function");

  pModule = PyImport_ExecCodeModule(module_name, pCompiledFn);

  if (pre_map_f != "") {
    pCompiledFn = Py_CompileString(pre_map_f.c_str(), "", Py_file_input);

    if (pCompiledFn == NULL)
      ERROR("[PY interpreter] I am not able to parse your function");

    pModule = PyImport_ExecCodeModule(module_name, pCompiledFn);
  }

  if (after_map_f != "") {
    pCompiledFn = Py_CompileString(after_map_f.c_str(), "", Py_file_input);

    if (pCompiledFn == NULL)
      ERROR("[PY interpreter] I am not able to parse your function");

    pModule = PyImport_ExecCodeModule(module_name, pCompiledFn);
  }

  if (pModule == NULL)
    ERROR("[PY interpreter] I am not able to create a module for your function");

  python_module = pModule;
}
// }}}
// pre_map {{{
void task_python::pre_map(TaskOptions& options) {
  if (pre_map_f == "")
    return;

  PyObject  *key = NULL, *value = NULL, *pFunc = NULL;
  Py_ssize_t pos = 0;
  PyObject* pOptions = PyDict_New();

  pFunc  = PyObject_GetAttrString(python_module, "pre_map" ) ;
  PyObject_CallFunctionObjArgs(pFunc, pOptions);

  if (pOptions == NULL) {
    ERROR("Python map did not return anything :( ");
  }

  // Save dictionary values as a PyObject pointer
  while (PyDict_Next(pOptions, &pos, &key, &value)) {
    string k = PyString_AsString(key);
    Py_INCREF(value);

    if (options.find(k) != options.end()) {
      options[k] = value;

    } else {
      options.insert({k, value});
    }
  }

  Py_DECREF(pOptions);
}
//}}}
// after_map {{{
void task_python::after_map(TaskOptions& options) {
  if(after_map_f != "")
    return;

  PyObject  *pFunc = NULL;
  PyObject* pOptions = PyDict_New();

  for (auto kv_pair : options) {
    PyObject* current_key = PyString_FromString(kv_pair.first.c_str());
    PyDict_SetItem(pOptions, current_key, static_cast<PyObject*>(kv_pair.second));
  }

  pFunc = PyObject_GetAttrString(python_module, "after_map" ) ;
  PyObject_CallFunctionObjArgs(pFunc, pOptions);

  if (pOptions == NULL) {
    ERROR("Python map did not return anything :( ");
  }

  Py_DECREF(pOptions);
}
// }}}
// map {{{
// @brief it runs the map function in python
//
// The python map function returns a dictionary
// we iterate the dictionary to return all the 
// key pair functions
//
// @todo multiples key values
void task_python::map(std::string& line, TaskOutput& out, TaskOptions& options) {
  PyObject  *key = NULL, *value = NULL, *pFunc = NULL;
  Py_ssize_t pos = 0;

  PyObject* pOptions = PyDict_New();
  PyObject* pOutput  = PyDict_New();

  for (auto kv_pair : options) {
    PyObject* current_key = PyString_FromString(kv_pair.first.c_str());
    PyDict_SetItem(pOptions, current_key, static_cast<PyObject*>(kv_pair.second));
  }

  pFunc = PyObject_GetAttrString(python_module, "map" ) ;
  PyObject_CallFunction(pFunc, (char*)"soo", const_cast<char*>(line.c_str()), 
      pOutput, pOptions);


  if (pOutput == NULL) {
    ERROR("Python map did not return anything :( ");
  }

  // Save dictionary values as a PyObject pointer
  while (PyDict_Next(pOutput, &pos, &key, &value)) {
    string k = PyString_AsString(key);

    for (Py_ssize_t i = 0; i < PyList_Size(value); i++) {
      PyObject* item = PyList_GetItem(value, i);
      string v = PyString_AsString(item);
      out.insert(k, v);
    }
  }

  Py_DECREF(pOutput);
}
// }}}
// reduce {{{
// @brief it runs the reduce python function
//
void task_python::reduce(std::string& key, vec_str& values, TaskOutput& out) {
  PyObject  *pKey = NULL, *pValue = NULL, *pFunc = NULL;
  Py_ssize_t pos = 0;

  PyObject* pOutput  = PyDict_New();
  PyObject* pInput = PyList_New(values.size());

  for (auto& value : values) {
    PyObject* pValue = PyString_FromString(value.c_str());
    PyList_Append(pInput, pValue);
  }

  pFunc = PyObject_GetAttrString(python_module, "reduce");
  PyObject_CallFunction(pFunc, (char*)"soo", const_cast<char*>(key.c_str()), 
      pInput, pOutput);


  if (pOutput == NULL) {
    ERROR("Python map did not return anything :( ");
  }

  // Save dictionary values as a PyObject pointer
  while (PyDict_Next(pOutput, &pos, &pKey, &pValue)) {
    string k = PyString_AsString(pKey);

    for (Py_ssize_t i = 0; i < PyList_Size(pValue); i++) {
      PyObject* item = PyList_GetItem(pValue, i);
      string v = PyString_AsString(item);
      out.insert(k, v);
    }
  }

  Py_DECREF(pOutput);
  Py_DECREF(pInput);
}
// }}}
