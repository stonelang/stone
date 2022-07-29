#ifndef STONE_SYNTAX_USING_H
#define STONE_SYNTAX_USING_H

namespace stone {
namespace syn {

class UsingPath {};

enum class UsingKind : uint8_t {
  /// using STD.IO;
  Module = 0,

  ///
  Type,
  /// using STD.IO.OutputStream;
  Struct,
  ///
  Enum,
  /// using STD.IO.Stream;
  Interface,

  //
  Auto,

  //
  Fun,

  //
  Alias,
};

} // namespace syn
} // namespace stone

#endif
