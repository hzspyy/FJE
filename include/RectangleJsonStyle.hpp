
#pragma once
#include <IconStyle.hpp>
#include <JsonStyle.hpp>
#include <Node.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class RectangleJson : public Container {
 public:
  void draw();
  RectangleJson(json& j) : Container(j) {}

 private:
  void drawHelper(std::unique_ptr<Node>& node, int level, bool isFirstRoot,
                  bool isLastRoot, int maxLength);
  int calculateMaxLength(std::unique_ptr<Node>& node, int level);
  void printLine(string content, int level, bool isFirstLine, bool isLastLine,
                 int maxLength);
};

class RectangleJsonStyleFactory : public JsonStyleFactory {
 public:
  std::unique_ptr<Container> create(json& j) override {
    return std::make_unique<RectangleJson>(j);
  }
};
