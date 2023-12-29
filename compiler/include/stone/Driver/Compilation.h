#ifndef STONE_DRIVER_DRIVER_COMPILATION_H
#define STONE_DRIVER_DRIVER_COMPILATION_H

#include "stone/Driver/Driver.h"
#include "stone/Driver/Job.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Chrono.h"

#include <memory>
#include <vector>

namespace llvm {
namespace opt {
class InputArgList;
class DerivedArgList;
} // namespace opt
} // namespace llvm

namespace stone {
class Job;
class JobConstruction;
class DiagnosticEngine;

class Compilation final {
  const Driver &driver;

  /// The Jobs which will be performed by this compilation.
  llvm::SmallVector<const Job *, 32> jobs;

  /// When the build was started.
  ///
  /// This should be as close as possible to when the driver was invoked, since
  /// it's used as a lower bound.
  llvm::sys::TimePoint<> compilationStartTime;

  /// The time of the last compilation.
  ///
  /// If unknown, this will be some time in the past.
  llvm::sys::TimePoint<> compilationLastTime = llvm::sys::TimePoint<>::min();

public:
  Compilation(const Driver &driver);
  // virtual ~Compilation();

public:
  // CompilationKind GetKind() const;
  const Driver &GetDriver() const { return driver; }

public:
  // virtual Status Execute() = 0;
};

// class QuadraticCompilation final : public Compilation {

// public:
//   QuadraticCompilation(Driver &driver);

// public:
//   Status BuildTopLevelJobConstructions() override;

// public:
//   Status Execute() override;

// public:
//   static bool classof(const Compilation *compilation) {
//     return compilation->GetKind() == CompilationKind::Quadratic;
//   }
// };

// class FlatCompilation : public Compilation {

// public:
//   FlatCompilation(Driver &driver);

// public:
//   Status BuildTopLevelJobConstructions() override;

// public:
//   Status Execute() override;

// public:
//   static bool classof(const Compilation *compilation) {
//     return compilation->GetKind() == CompilationKind::Flat;
//   }
// };

// class CPUCountCompilation : public Compilation {

// public:
//   CPUCountCompilation(Driver &driver);

// public:
//   Status BuildTopLevelJobConstructions() override;

// public:
//   Status Execute() override;

// public:
//   static bool classof(const Compilation *compilation) {
//     return compilation->GetKind() == CompilationKind::CPUCount;
//   }
// };

// class SingleCompilation : public Compilation {

// public:
//   SingleCompilation(Driver &driver);

// public:
//   Status BuildTopLevelJobConstructions() override;

// public:
//   Status Execute() override;

// public:
//   static bool classof(const Compilation *compilation) {
//     return compilation->GetKind() == CompilationKind::Single;
//   }
// };

} // namespace stone

#endif
