#ifndef STONE_DIAG_ALL_DIAGNOSTICSCOMMON_H
#define STONE_DIAG_ALL_DIAGNOSTICSCOMMON_H

#include "stone/Diag/DiagnosticBasicKind.h"
#include "stone/Diag/DiagnosticParseKind.h"

namespace stone {
namespace diags {
template <size_t SizeOfStr, typename FieldType> class StringSizerHelper {
  static_assert(SizeOfStr <= FieldType(~0U), "Field too small!");

public:
  enum { Size = SizeOfStr };
};
} // namespace diags

} // namespace stone
#define STR_SIZE(str, fieldTy)                                                 \
  stone::diags::StringSizerHelper<sizeof(str) - 1, fieldTy>::Size

#endif
