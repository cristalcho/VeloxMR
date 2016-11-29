#include "map_output_collection.hh"
#include <iostream>

namespace velox {
  MapOutputCollection::MapOutputCollection() {
    collection_ = nullptr;
  }

  MapOutputCollection::~MapOutputCollection() {
    if(collection_ != nullptr) {
      for(auto iter = collection_->begin(); iter != collection_->end(); ++iter) 
        delete iter->second;

      delete collection_;
    }
  }

  bool MapOutputCollection::insert(std::string key, std::string value) {
    this->check_or_alloc_collection();

    auto collection_item = collection_->find(key);

    if(collection_item != collection_->end())
      (reinterpret_cast<value_t>(collection_item->second))->push_back(value);
    else
      collection_->insert(std::pair<key_t, value_t>(key, new std::vector<std::string>(1, value)));

    return true;
  };

  void MapOutputCollection::check_or_alloc_collection() {
    if(collection_ == nullptr) 
      collection_ = new std::map<key_t, value_t>();
  }

  auto MapOutputCollection::begin() {
    this->check_or_alloc_collection();
    return collection_->begin();
  }

  auto MapOutputCollection::end() {
    this->check_or_alloc_collection();
    return collection_->end();
  }

  void MapOutputCollection::travel(std::function<void(std::string, std::string)> run_block_with_kv) {
    for(auto key_values = this->begin(); key_values != this->end(); ++key_values) {
      for(auto value_ptr = key_values->second->begin(); value_ptr != key_values->second->end(); ++value_ptr)
        run_block_with_kv(key_values->first, *value_ptr);
    }
  }

  void MapOutputCollection::print_all() {
    travel([](std::string k, std::string v) {
      std::cout << "<" << k << ", " << v << ">" << std::endl;
    });
  }
}
