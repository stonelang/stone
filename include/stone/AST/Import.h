#ifndef STONE_AST_IMPORT_H
#define STONE_AST_IMPORT_H

// #include "stone/Basic/Located.h"

#include "stone/Basic/Basic.h"

namespace stone {

// TODO: You may not need this part
enum class ImportKind : uint8 {
  /// import Core.IO;
  Module = 0,
  /// import type Core.IO.OutputStream;
  Type,
  /// import struct Core.Time.Time;
  Struct,
  /// import enum Core.Time.Day;
  Enum,
  /// import interface Core.Collections.Collection;
  Interface,
  // For variables
  Auto,
  ///// import fun System.Math.Max
  Fun,
};

class ImportSearchPathBase {
public:
};

class ImportSearchPath final : public ImportSearchPathBase {
public:
};

} // namespace stone

#endif
