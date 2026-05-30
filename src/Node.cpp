#include <Node.hpp>

#include <cstddef>
#include <string>

namespace {

std::string iconPrefix(bool enabled, const std::string& value) {
  return enabled ? value : "";
}

std::string arrayKey(size_t index) { return "[" + std::to_string(index) + "]"; }

std::string scalarValue(const json& value) {
  if (value.is_null()) {
    return "null";
  }
  return value.dump();
}

void appendJson(Container& parent, const std::string& key, const json& value,
                unsigned level) {
  if (value.is_structured()) {
    parent.children.push_back(std::make_unique<Container>(value, key, level));
    return;
  }

  parent.children.push_back(std::make_unique<Leaf>(key, scalarValue(value)));
}

}  // namespace

string Leaf::render(const IconStyle& icon) const {
  return iconPrefix(icon.enabled, icon.leaf) + key + ": " + value;
}

string Container::render(const IconStyle& icon) const {
  return iconPrefix(icon.enabled, icon.container) + key;
}

Container::Container(const json& j, string key, unsigned level) {
  this->level = level;
  this->key = key;

  if (j.is_object()) {
    for (auto it = j.begin(); it != j.end(); ++it) {
      appendJson(*this, it.key(), it.value(), level + 1);
    }
    return;
  }

  if (j.is_array()) {
    for (size_t i = 0; i < j.size(); ++i) {
      appendJson(*this, arrayKey(i), j.at(i), level + 1);
    }
    return;
  }

  children.push_back(std::make_unique<Leaf>("value", scalarValue(j)));
}
