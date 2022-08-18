#ifndef STONE_SYNTAX_USING_H
#define STONE_SYNTAX_USING_H

namespace stone {
namespace syn {

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
  //
  Auto,
  //
  Fun,
  //
  Alias,
};

class UsingPathBase {};

class UsingPath {};

} // namespace syn
} // namespace stone

#endif
