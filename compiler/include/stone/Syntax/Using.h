#ifndef STONE_SYNTAX_USING_H
#define STONE_SYNTAX_USING_H

// #include "stone/Basic/Located.h"

namespace stone {
namespace syn {

// TODO: You may not need this part
enum class UsingKind : uint8_t {
  /// using STD.IO;
  Module = 0,
  ///
  Type,
  /// using STD.Core.IO.OutputStream;
  Struct,
  ///
  Enum,
  /// using STD.Core.IO.Stream;
  Interface,
  // For variables
  Auto,
  //
  Fun,
};

class UsingSearchPathBase {};

class UsingSearchPath final : public UsingSearchPathBase {};

} // namespace syn
} // namespace stone

#endif
