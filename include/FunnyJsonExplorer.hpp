#pragma once
#include <JsonStyle.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <ostream>
#include <utility>

#include "Node.hpp"

using json = nlohmann::json;

// FJE 程序
class FunnyJsonExplorer {
 public:
  FunnyJsonExplorer(std::unique_ptr<JsonStyleFactory>&& jf, IconStyle icon)
      : jf(std::move(jf)), icon(std::move(icon)) {}
  void show(const json& j, std::ostream& out) {
    std::unique_ptr<Container> ct = load(j);
    ct->draw(out, icon);
  }

 private:
  std::unique_ptr<Container> load(const json& j) { return jf->create(j); }
  std::unique_ptr<JsonStyleFactory> jf;
  IconStyle icon;
};
