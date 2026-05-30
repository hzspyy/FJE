#include <RectangleJsonStyle.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
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

void RectangleJson::draw() {
  int maxLength = 0;
  for (auto&& child : children) {
    maxLength = std::max(maxLength, calculateMaxLength(child, 1));
  }

  for (size_t i = 0; i < children.size(); i++) {
    drawHelper(children[i], 1, i == 0, i == children.size() - 1, maxLength);
  }
}

void RectangleJson::drawHelper(std::unique_ptr<Node>& node, int level,
                               bool isFirstRoot, bool isLastRoot,
                               int maxLength) {
  if (node == nullptr) {
    return;
  }
  std::string content = node->render();
  Container* container = dynamic_cast<Container*>(node.get());
  bool isLastLine =
      isLastRoot && (container == nullptr || container->children.empty());
  bool isFirstLine = isFirstRoot;

  printLine(content, level, isFirstLine, isLastLine, maxLength);

  if (container != nullptr) {
    for (size_t i = 0; i < container->children.size(); i++) {
      drawHelper(container->children[i], level + 1, false,
                 isLastRoot && i == container->children.size() - 1, maxLength);
    }
  }
}

int RectangleJson::calculateMaxLength(std::unique_ptr<Node>& node, int level) {
  if (node == nullptr) {
    return 0;
  }

  int maxLength = utf8CodePointCount(node->render()) + level * 8;
  Container* container = dynamic_cast<Container*>(node.get());
  if (container != nullptr) {
    for (auto&& child : container->children) {
      maxLength = std::max(maxLength, calculateMaxLength(child, level + 1));
    }
  }
  return maxLength;
}

void RectangleJson::printLine(std::string content, int level, bool isFirst,
                              bool isLast, int maxLength) {
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
    std::cout << line << "┐\n";
  } else if (isLast) {
    std::cout << line << "┘\n";
  } else {
    std::cout << line << "┤\n";
  }
}

struct RectangleJsonStyleFactoryRegistrar {
  RectangleJsonStyleFactoryRegistrar() {
    JsonFactory::registerFactory("rectangle", [] {
      return std::make_unique<RectangleJsonStyleFactory>();
    });
  }
} rectangleJsonStyleFactoryRegistrar;  // 全局变量，构造函数在程序启动时运行
