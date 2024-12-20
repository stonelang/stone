#ifndef STONE_DIAG_PARTIALDIAGNOSTIC_H
#define STONE_DIAG_PARTIALDIAGNOSTIC_H

#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include <cassert>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

namespace stone {
namespace diags {
class PartialInFlightDiagnostic : public StreamingDiagnostic {
private:
};
} // namespace diags
} // namespace stone
#endif