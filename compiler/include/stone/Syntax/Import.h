#ifndef STONE_SYNTAX_IMPORT_H
#define STONE_SYNTAX_IMPORT_H

// #include "stone/Basic/Located.h"

#include "stone/Basic/STDTypeAlias.h"

namespace stone {
namespace syn {

// TODO: You may not need this part
enum class ImportKind : UInt8 {
  /// import STD.IO;
  Module = 0,
  ///
  Type,
  /// import STD.Core.IO.OutputStream;
  Struct,
  ///
  Enum,
  /// import STD.Core.IO.Stream;
  Interface,
  // For variables
  Auto,
  //
  Fun,
};

class ImportSearchPathBase {};

class ImportSearchPath final : public ImportSearchPathBase {
public:
};

} // namespace syn
} // namespace stone

#endif
