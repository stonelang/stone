#ifndef STONE_SYNTAX_SEARCHPATH_OPTIONS_H
#define STONE_SYNTAX_SEARCHPATH_OPTIONS_H

namespace stone {

enum class SearchPathKind : uint8_t {
  Using,
  Framework,
  RuntimeLibrary,
};
/// Options for controlling diagnostics.
class SearchPathOptions final {
public:
};

} // namespace stone

#endif
