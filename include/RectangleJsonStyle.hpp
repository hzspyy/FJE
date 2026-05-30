#pragma once
#include <IconStyle.hpp>
#include <JsonStyle.hpp>
#include <Node.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>

using json = nlohmann::json;

class RectangleJson : public Container {
 public:
  void draw(std::ostream& out, const IconStyle& icon) const override;
  RectangleJson(const json& j) : Container(j) {}

 private:
  void drawHelper(std::ostream& out, const std::unique_ptr<Node>& node,
                  int level, bool isFirstRoot, bool isLastRoot, int maxLength,
                  const IconStyle& icon) const;
  int calculateMaxLength(const std::unique_ptr<Node>& node, int level,
                         const IconStyle& icon) const;
  void printLine(std::ostream& out, string content, int level, bool isFirstLine,
                 bool isLastLine, int maxLength) const;
};

class RectangleJsonStyleFactory : public JsonStyleFactory {
 public:
  std::unique_ptr<Container> create(const json& j) override {
    return std::make_unique<RectangleJson>(j);
  }
};
