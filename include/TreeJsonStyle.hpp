#pragma once
#include <IconStyle.hpp>
#include <JsonStyle.hpp>
#include <Node.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>

using json = nlohmann::json;

class TreeJson : public Container {
 public:
  void draw(std::ostream& out, const IconStyle& icon) const override;
  TreeJson(const json& j) : Container(j) {}

 private:
  void drawHelper(std::ostream& out, const std::unique_ptr<Node>& node,
                  string indent, bool isLast, const IconStyle& icon) const;
};

class TreeJsonStyleFactory : public JsonStyleFactory {
 public:
  std::unique_ptr<Container> create(const json& j) override {
    return std::make_unique<TreeJson>(j);
  }
};
