#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/TaskQueue.h"
#include "stone/Driver/ToolChain.h"
#include "stone/Stats/Stats.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringRef.h"

#include <functional>
#include <memory>
#include <string>

namespace llvm {
namespace opt {
class Arg;
class ArgList;
class OptTable;
class InputArgList;
class DerivedArgList;
} // namespace opt
} // namespace llvm

namespace stone {

class Driver final {

  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};

  /// The allocator used to create Driver objects.
  /// Driver objects are never destructed; rather, all memory associated
  /// with the Driver objects will be released when the Driver
  /// itself is destroyed.
  mutable llvm::BumpPtrAllocator allocator;

public:
  Driver();
  ~Driver();

public:
public:
  /// Allocate - Allocate memory from the Driver bump pointer.
  void *Allocate(unsigned long bytes, unsigned alignment = 8) const {
    if (bytes == 0) {
      return nullptr;
    }
    return allocator.Allocate(bytes, alignment);
  }
};

} // namespace stone
#endif