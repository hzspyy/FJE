#pragma once
#include <IconStyle.hpp>
#include <JsonStyle.hpp>
#include <Node.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;


class TreeJson : public Container {
 public:
  void draw();
  TreeJson(json& j) : Container(j) {}

 private:
  void drawHelper(std::unique_ptr<Node>& node, string indent, bool isLast);
};

class TreeJsonStyleFactory : public JsonStyleFactory {
 public:
  std::unique_ptr<Container> create(json& j) override {
    return std::make_unique<TreeJson>(j);
  }
};
