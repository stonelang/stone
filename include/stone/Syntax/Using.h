#ifndef STONE_SYNTAX_USE_H
#define STONE_SYNTAX_USE_H

namespace stone {
namespace syn {

class UsingPath {};

enum class UsingKind : uint8_t {
  Module = 0,
  Type,
  Struct,
  Enum,
  Interface,
  Auto,
  Fun
};

} // namespace syn
} // namespace stone

#endif
