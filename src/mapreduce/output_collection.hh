#pragma once

#include <map>
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace velox {

      using key_t = std::string;
      using value_t = std::vector<std::string>;
  class OutputCollection {
    public:
      OutputCollection();
      ~OutputCollection();

      /* TODO: arguments for any types */
      bool insert(std::string, std::string);

      std::map<std::string, value_t>::iterator begin();
      std::map<std::string, value_t>::iterator end();

      /* TODO: arguments for any types */
      void travel(std::function<void(std::string, std::vector<std::string>)>);

      void print_all();

    private:

      std::map<key_t, value_t> collection_; 

      void check_or_alloc_collection();
  };
}
