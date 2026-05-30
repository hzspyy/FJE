#include <TreeJsonStyle.hpp>

void TreeJson::draw(std::ostream& out, const IconStyle& icon) const {
  for (size_t i = 0; i < children.size(); i++) {
    drawHelper(out, children[i], "", i == children.size() - 1, icon);
  }
}

void TreeJson::drawHelper(std::ostream& out, const std::unique_ptr<Node>& node,
                          string indent, bool isLast,
                          const IconStyle& icon) const {
  if (node == nullptr) {
    return;
  }

  out << indent;
  if (isLast) {
    out << "└─ ";
    indent += "   ";
  } else {
    out << "├─ ";
    indent += "│  ";
  }
  out << node->render(icon) << "\n";

  Container* container = dynamic_cast<Container*>(node.get());
  if (container != nullptr) {
    for (size_t i = 0; i < container->children.size(); i++) {
      drawHelper(out, container->children[i], indent,
                 i == container->children.size() - 1, icon);
    }
  }
}


struct TreeJsonStyleFactoryRegistrar {
  TreeJsonStyleFactoryRegistrar() {
    JsonFactory::registerFactory("tree", [] {
      return std::make_unique<TreeJsonStyleFactory>();
    });
  }
} treeJsonStyleFactoryRegistrar;  // 全局变量，构造函数在程序启动时运行
