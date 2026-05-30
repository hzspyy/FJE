#include <argparse/argparse.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "FunnyJsonExplorer.hpp"
#include "IconStyle.hpp"
#include "JsonStyle.hpp"

using json = nlohmann::json;

namespace {

constexpr char kDefaultIconConfigPath[] = "icon_config.json";

struct CliOptions {
  std::string inputPath;
  std::string outputPath;
  std::string style;
  std::string iconName;
  std::string iconConfigPath;
  bool iconConfigProvided = false;
  bool listIcons = false;
  bool noIcons = false;
};

json parseJson(std::istream& input, const std::string& description) {
  try {
    return json::parse(input);
  } catch (const json::parse_error& err) {
    throw std::runtime_error("Invalid " + description + ": " +
                             std::string(err.what()));
  }
}

json loadJsonFile(const std::string& path, const std::string& description) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error(description + " not found: " + path);
  }
  return parseJson(file, description);
}

json loadJsonInput(const std::string& path) {
  if (path == "-") {
    return parseJson(std::cin, "stdin json");
  }
  return loadJsonFile(path, "json file");
}

std::optional<std::filesystem::path> firstExistingPath(
    const std::vector<std::filesystem::path>& candidates) {
  for (const auto& candidate : candidates) {
    if (std::filesystem::exists(candidate)) {
      return candidate;
    }
  }
  return std::nullopt;
}

std::filesystem::path executablePath(const char* argv0) {
  std::filesystem::path path(argv0);
  if (path.is_relative()) {
    path = std::filesystem::absolute(path);
  }
  return path.lexically_normal();
}

std::string resolveIconConfigPath(const std::string& requestedPath,
                                  const char* argv0,
                                  bool explicitConfigPath) {
  std::filesystem::path requested(requestedPath);
  if (requested.is_absolute() || explicitConfigPath) {
    if (std::filesystem::exists(requested)) {
      return requested.string();
    }
    throw std::runtime_error("icon config not found: " + requested.string());
  }

  const std::filesystem::path exe = executablePath(argv0);
  std::vector<std::filesystem::path> candidates = {
      std::filesystem::current_path() / requested,
      exe.parent_path() / requested,
      exe.parent_path().parent_path() / requested,
  };

  std::optional<std::filesystem::path> found = firstExistingPath(candidates);
  if (found.has_value()) {
    return found->string();
  }

  std::ostringstream message;
  message << "icon config not found. Looked in:";
  for (const auto& candidate : candidates) {
    message << "\n  " << candidate.string();
  }
  throw std::runtime_error(message.str());
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

IconStyle createIconStyle(const json& iconConfig, const std::string& iconStyle) {
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

  return IconStyle{iconStyle, iconStyleConfig["leaf"].get<std::string>(),
                   iconStyleConfig["container"].get<std::string>(), true};
}

IconStyle noIconStyle() { return IconStyle{"none", "", "", false}; }

void printIcons(const json& iconConfig, std::ostream& out) {
  for (const auto& iconName : getIconNames(iconConfig)) {
    const json& icon = iconConfig.at(iconName);
    out << iconName << "  container=" << icon.value("container", "")
        << " leaf=" << icon.value("leaf", "") << "\n";
  }
}

void configureArgumentParser(argparse::ArgumentParser& program) {
  program.add_argument("-f", "--file")
      .help("json file to parse, or '-' to read stdin")
      .default_value(std::string(""));

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

  program.add_argument("-o", "--output")
      .help("Output to a file")
      .default_value(std::string(""));

  program.add_argument("--list-icons")
      .help("List icons from the resolved icon config and exit")
      .default_value(false)
      .implicit_value(true);

  program.add_argument("--no-icons")
      .help("Render without configured icon prefixes")
      .default_value(false)
      .implicit_value(true);
}

CliOptions readOptions(const argparse::ArgumentParser& program) {
  return CliOptions{
      program.get<std::string>("--file"),
      program.get<std::string>("--output"),
      program.get<std::string>("--style"),
      program.get<std::string>("--icon"),
      program.get<std::string>("--icon-config"),
      program.is_used("--icon-config"),
      program.get<bool>("--list-icons"),
      program.get<bool>("--no-icons"),
  };
}

std::unique_ptr<JsonStyleFactory> createStyleFactory(
    const std::string& style) {
  std::unique_ptr<JsonStyleFactory> factory = JsonFactory::getFactory(style);
  if (factory == nullptr) {
    throw std::runtime_error("Style factory not registered: " + style);
  }
  return factory;
}

std::ostream& outputStream(const std::string& outputPath,
                           std::ofstream& outputFile) {
  if (outputPath.empty()) {
    return std::cout;
  }

  outputFile.open(outputPath);
  if (!outputFile.is_open()) {
    throw std::runtime_error("output file could not be opened: " + outputPath);
  }
  return outputFile;
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
    CliOptions options = readOptions(program);

    if (options.inputPath.empty() && !options.listIcons) {
      std::cout << "No input file provided. Use -f <path> or -f - for stdin.\n";
      std::cout << program;
      return 1;
    }

    std::ofstream outputFile;
    std::ostream& out = outputStream(options.outputPath, outputFile);

    IconStyle icon = noIconStyle();
    if (!options.noIcons || options.listIcons) {
      const std::string iconConfigPath = resolveIconConfigPath(
          options.iconConfigPath, argv[0], options.iconConfigProvided);
      json iconConfig = loadJsonFile(iconConfigPath, "icon config");

      if (options.listIcons) {
        printIcons(iconConfig, out);
        return 0;
      }

      icon = createIconStyle(iconConfig, options.iconName);
    }

    json data = loadJsonInput(options.inputPath);
    std::unique_ptr<JsonStyleFactory> factory = createStyleFactory(options.style);

    FunnyJsonExplorer fje(std::move(factory), icon);
    fje.show(data, out);
  } catch (const std::runtime_error& err) {
    std::cout << err.what() << std::endl;
    return 1;
  }
}
