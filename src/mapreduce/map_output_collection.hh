#pragma once

#include <map>
#include <vector>
#include <string>
#include <functional>

namespace velox {
  class MapOutputCollection {
    public:
      MapOutputCollection();
      ~MapOutputCollection();

      /* TODO: arguments for any types */
      bool insert(std::string, std::string);

      auto begin();
      auto end();

      /* TODO: arguments for any types */
      void travel(std::function<void(std::string, std::vector<std::string>*)>);

      void print_all();

    private:
      using key_t = std::string;
      using value_t = std::vector<std::string>*;

      std::map<key_t, value_t>* collection_; 

      void check_or_alloc_collection();
  };
}
