#pragma once
#include <IconStyle.hpp>
#include <Node.hpp>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

class JsonStyleFactory {
 public:
  virtual std::unique_ptr<Container> create(json& j) = 0;
  virtual ~JsonStyleFactory() = default;
};

class JsonFactory {
 public:
  using FactoryCreator = std::function<std::unique_ptr<JsonStyleFactory>()>;

  static void registerFactory(const std::string& style,
                              FactoryCreator creator) {
    getRegistry()[style] = std::move(creator);
  }

  static std::unique_ptr<JsonStyleFactory> getFactory(
      const std::string& style) {
    auto& registry = getRegistry();
    auto it = registry.find(style);
    if (it != registry.end()) {
      return it->second();
    }
    return nullptr;
  }

 private:
  static std::unordered_map<std::string, FactoryCreator>& getRegistry() {
    static std::unordered_map<std::string, FactoryCreator> registry;
    return registry;
  }
};
