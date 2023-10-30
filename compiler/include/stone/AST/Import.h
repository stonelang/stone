#ifndef STONE_ASTIMPORT_H
#define STONE_ASTIMPORT_H

// #include "stone/Basic/Located.h"

#include "stone/Basic/STDAlias.h"

namespace stone {
namespace ast {

// TODO: You may not need this part
enum class ImportKind : UInt8 {
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

class ImportSearchPathBase {};

class ImportSearchPath final : public ImportSearchPathBase {
public:
};

} // namespace ast
} // namespace stone

#endif
