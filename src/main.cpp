#include <argparse/argparse.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

#include "FunnyJsonExplorer.hpp"
#include "IconStyle.hpp"
#include "JsonStyle.hpp"

using json = nlohmann::json;

namespace {

constexpr char kDefaultIconConfigPath[] = "icon_config.json";

json loadJsonFile(const std::string& path, const std::string& description) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error(description + " not found: " + path);
  }

  try {
    return json::parse(file);
  } catch (const json::parse_error& err) {
    throw std::runtime_error("Invalid " + description + ": " +
                             std::string(err.what()));
  }
}

std::vector<std::string> getIconNames(const json& iconConfig) {
  if (!iconConfig.is_object()) {
    throw std::runtime_error("Icon config must be a JSON object.");
  }

  std::vector<std::string> iconNames;
  iconNames.reserve(iconConfig.size());
  for (auto it = iconConfig.begin(); it != iconConfig.end(); ++it) {
    iconNames.push_back(it.key());
  }
  return iconNames;
}

std::string joinIconNames(const std::vector<std::string>& iconNames) {
  std::string joined;
  for (const auto& name : iconNames) {
    if (!joined.empty()) {
      joined += ", ";
    }
    joined += name;
  }
  return joined;
}

void configureIcon(const json& iconConfig, const std::string& iconStyle) {
  const auto iconIt = iconConfig.find(iconStyle);
  if (iconIt == iconConfig.end()) {
    throw std::runtime_error("Invalid icon: " + iconStyle +
                             ". Available icons: " +
                             joinIconNames(getIconNames(iconConfig)));
  }

  const json& iconStyleConfig = iconIt.value();
  if (!iconStyleConfig.contains("leaf") ||
      !iconStyleConfig.contains("container") ||
      !iconStyleConfig["leaf"].is_string() ||
      !iconStyleConfig["container"].is_string()) {
    throw std::runtime_error("Icon style '" + iconStyle +
                             "' must define string values for leaf and "
                             "container.");
  }

  Icon& icon = Icon::getInstance();
  icon.leafIcon = iconStyleConfig["leaf"].get<std::string>();
  icon.containerIcon = iconStyleConfig["container"].get<std::string>();
  icon.name = iconStyle;
}

void configureArgumentParser(argparse::ArgumentParser& program) {
  program.add_argument("-f", "--file").required().help("json file to be parsed");

  program.add_argument("-s", "--style")
      .help("Choose a display style: rectangle or tree")
      .default_value(std::string("tree"))
      .action([](const std::string& value) {
        if (value != "rectangle" && value != "tree") {
          throw std::runtime_error("Invalid style: " + value);
        }
        return value;
      });

  program.add_argument("-i", "--icon")
      .help("Choose an icon style from the icon config")
      .default_value(std::string("star"));

  program.add_argument("-c", "--icon-config")
      .help("Path to icon config json")
      .default_value(std::string(kDefaultIconConfigPath));

  // TODO: output to a file.
  program.add_argument("-o", "--output")
      .help("Output to a file")
      .default_value(std::string(""));
}

std::unique_ptr<JsonStyleFactory> createStyleFactory(
    const std::string& style) {
  std::unique_ptr<JsonStyleFactory> factory = JsonFactory::getFactory(style);
  if (factory == nullptr) {
    throw std::runtime_error("Style factory not registered: " + style);
  }
  return factory;
}

}  // namespace

int main(int argc, char** argv) {
  argparse::ArgumentParser program("FJE");
  configureArgumentParser(program);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    return 1;
  }

  try {
    const std::string jsonFilePath = program.get<std::string>("--file");
    const std::string style = program.get<std::string>("--style");
    const std::string iconStyle = program.get<std::string>("--icon");
    const std::string iconConfigPath =
        program.get<std::string>("--icon-config");

    json iconConfig = loadJsonFile(iconConfigPath, "icon config");
    configureIcon(iconConfig, iconStyle);

    json data = loadJsonFile(jsonFilePath, "json file");
    std::unique_ptr<JsonStyleFactory> factory = createStyleFactory(style);

    FunnyJsonExplorer fje(std::move(factory));
    fje.show(data);
  } catch (const std::runtime_error& err) {
    std::cout << err.what() << std::endl;
    return 1;
  }
}
