#pragma once
#include <IconStyle.hpp>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;
using std::string;
using std::vector;

struct Node {
  string key;
  virtual string render(const IconStyle& icon) const = 0;
  virtual ~Node() = default;
};

struct Leaf : Node {
  string value;
  string render(const IconStyle& icon) const override;
  Leaf(string key, string value) : value(value) { this->key = key; }
};

struct Container : Node {
  vector<std::unique_ptr<Node>> children;
  unsigned level;

  string render(const IconStyle& icon) const override;
  Container(const json& j, string key = "", unsigned level = 0);

  virtual void draw(std::ostream& out, const IconStyle& icon) const {
    std::cerr << "Error: draw() not implemented for this container.\n";
  }

  Container(const Container&) = delete;
  Container& operator=(const Container&) = delete;
  virtual ~Container() = default;
};
