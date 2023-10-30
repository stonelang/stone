#ifndef STONE_FOREIGN_FOREIGN_H
#define STONE_FOREIGN_FOREIGN_H

#include "stone/Basic/STDAlias.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
//  Only C for the time being.
/// Enumeration describing foreign languages to which Stone may be bridged.
enum class ForeignLangKind : UInt8 {
  None = 0,
  C,
};

} // namespace stone
#endif
