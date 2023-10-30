#ifndef STONE_AST_SEARCHPATH_H
#define STONE_AST_SEARCHPATH_H

namespace stone {

enum class SearchPathKind : unsigned {
  Using,
  Framework,
  RuntimeLibrary,
};
/// Options for controlling diagnostics.
class SearchPathOptions final {
public:
};

class SearchPath final {
public:
  SearchPathOptions SearchPathOpts;

public:
  SearchPath();
  ~SearchPath();
};

} // namespace stone

#endif
