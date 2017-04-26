#include "output_collection.hh"
#include <iostream>
#include <memory>

using std::make_shared;

namespace velox {

OutputCollection::OutputCollection() { }
OutputCollection::~OutputCollection() { }

bool OutputCollection::insert(std::string key, std::string value) {

  auto collection_item = collection_.find(key);

  if (collection_item == collection_.end()) {
    collection_item = collection_.insert({key, {}}).first;
    collection_item->second.reserve(2048);
  }

  collection_item->second.push_back(value);

  return true;
};

void OutputCollection::check_or_alloc_collection() { }

std::map<std::string, value_t>::iterator OutputCollection::begin() { return collection_.begin(); }
std::map<std::string, value_t>::iterator OutputCollection::end() { return collection_.end(); }

void OutputCollection::travel(
    std::function<void(std::string, value_t)> run_block_with_kv) {
  for(auto key_values = begin(); key_values != end(); ++key_values) {
    run_block_with_kv(key_values->first, key_values->second);
  }
}

void OutputCollection::print_all() { }
}
