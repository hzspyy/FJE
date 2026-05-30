#include <RectangleJsonStyle.hpp>

#include <algorithm>
#include <cassert>
#include <string_view>

namespace {

int utf8CodePointCount(std::string_view text) {
  int count = 0;
  for (const unsigned char ch : text) {
    if ((ch & 0xC0) != 0x80) {
      ++count;
    }
  }
  return count;
}

}  // namespace

void RectangleJson::draw(std::ostream& out, const IconStyle& icon) const {
  int maxLength = 0;
  for (auto&& child : children) {
    maxLength = std::max(maxLength, calculateMaxLength(child, 1, icon));
  }

  for (size_t i = 0; i < children.size(); i++) {
    drawHelper(out, children[i], 1, i == 0, i == children.size() - 1,
               maxLength, icon);
  }
}

void RectangleJson::drawHelper(std::ostream& out,
                               const std::unique_ptr<Node>& node, int level,
                               bool isFirstRoot, bool isLastRoot, int maxLength,
                               const IconStyle& icon) const {
  if (node == nullptr) {
    return;
  }
  std::string content = node->render(icon);
  Container* container = dynamic_cast<Container*>(node.get());
  bool isLastLine =
      isLastRoot && (container == nullptr || container->children.empty());
  bool isFirstLine = isFirstRoot;

  printLine(out, content, level, isFirstLine, isLastLine, maxLength);

  if (container != nullptr) {
    for (size_t i = 0; i < container->children.size(); i++) {
      drawHelper(out, container->children[i], level + 1, false,
                 isLastRoot && i == container->children.size() - 1, maxLength,
                 icon);
    }
  }
}

int RectangleJson::calculateMaxLength(const std::unique_ptr<Node>& node,
                                      int level,
                                      const IconStyle& icon) const {
  if (node == nullptr) {
    return 0;
  }

  int maxLength = utf8CodePointCount(node->render(icon)) + level * 8;
  Container* container = dynamic_cast<Container*>(node.get());
  if (container != nullptr) {
    for (auto&& child : container->children) {
      maxLength =
          std::max(maxLength, calculateMaxLength(child, level + 1, icon));
    }
  }
  return maxLength;
}

void RectangleJson::printLine(std::ostream& out, std::string content, int level,
                              bool isFirst, bool isLast,
                              int maxLength) const {
  std::string indent = "";
  for (int i = 0; i < level - 1; i++) {
    indent += "│  ";
  }
  std::string prefix = isFirst ? "┌─ " : (isLast ? "└─ " : "├─ ");
  std::string line = indent + prefix + content;

  int padding = maxLength - utf8CodePointCount(line);
  assert(padding >= 0 && "padding must be non-negative");
  line += std::string(padding, '-');

  if (isFirst) {
    out << line << "┐\n";
  } else if (isLast) {
    out << line << "┘\n";
  } else {
    out << line << "┤\n";
  }
}

struct RectangleJsonStyleFactoryRegistrar {
  RectangleJsonStyleFactoryRegistrar() {
    JsonFactory::registerFactory("rectangle", [] {
      return std::make_unique<RectangleJsonStyleFactory>();
    });
  }
} rectangleJsonStyleFactoryRegistrar;  // 全局变量，构造函数在程序启动时运行
