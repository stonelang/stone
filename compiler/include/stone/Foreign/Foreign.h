#ifndef STONE_FOREIGN_FOREIGN_H
#define STONE_FOREIGN_FOREIGN_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
//  Only C for the time being.
/// Enumeration describing foreign languages to which Stone may be bridged.
enum class ForeignLangKind {
  C,
};

} // namespace stone
#endif
