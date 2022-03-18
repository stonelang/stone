#ifndef STONE_COMPILATIONJOB_H
#define STONE_COMPILATIONJOB_H

#include "stone/Core/Color.h"
#include "stone/Core/List.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/Command.h"
#include "stone/Driver/CrashState.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Intent.h"
#include "stone/Driver/JobKind.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/Timer.h"

namespace stone {
class Tool;
class Driver;
class CompilationJob;

// The process ID
using CompilationJobID = int64_t;

enum class CompilationJobKind {
  None = 0,
  Compile,
  DynamicLink,
  StaticLink,
  First = Compile,
  Last = StaticLink,
};
enum class CompilationJobLevel {
  None = 0,
  Top,
  Dep,
};

using Inputs = llvm::ArrayRef<const file::File *>;
using Deps = llvm::ArrayRef<const CompilationJob *>;

class CompilationJob : public Command {

  friend class Compilation;

  CompilationJobID jobID;
  CompilationJobKind kind;
  CompilationJobLevel level;

  llvm::TinyPtrVector<const file::File *> inputs;
  llvm::TinyPtrVector<const CompilationJob *> deps;

public:
  CompilationJob(CompilationJobKind kind, CompilationJobLevel level,
                 const Tool &tool, Inputs inputs)
      : Command(tool), kind(kind), level(level), inputs(inputs) {}

  CompilationJob(CompilationJobKind kind, CompilationJobLevel level,
                 const Tool &tool, Deps deps)
      : Command(tool), kind(kind), level(level), deps(deps) {}

public:
  virtual void Run() {}

protected:
  virtual Command *ToCommand() const { return nullptr; }

public:
  llvm::ArrayRef<const file::File *> GetInputs() { return inputs; }
  llvm::TinyPtrVector<const CompilationJob *> GetDeps() { return deps; }

  CompilationJobKind GetKind() const { return kind; }
  CompilationJobLevel GetLevel() const { return level; }

public:
  // Required for llvm::dyn_cast
  static bool classof(const CompilationJob *job) {
    return (job->GetKind() >= CompilationJobKind::First &&
            job->GetKind() <= CompilationJobKind::Last);
  }
};

// class CompileJob final : public CompilationJob {
// public:
//   CompileJob(CompilationJobLevel level, Inputs inputs)
//       : CompilationJob(CompilationJobKind::Compile, level, inputs) {}

// public:
//   //Command *ToCommand() const override { return this; }

// public:
//   static bool classof(const CompilationJob *job) {
//     return job->GetKind() == CompilationJobKind::Compile;
//   }
// };

// class DynamicLinkJob final : public CompilationJob {
//   bool requiresLTO;

// public:
//   DynamicLinkJob(Inputs inputs, bool requiresLTO = false)
//       : CompilationJob(CompilationJobKind::DynamicLink,
//                        CompilationJobLevel::Top, inputs),
//         requiresLTO(requiresLTO) {}

//   DynamicLinkJob(Deps deps, bool requiresLTO = false)
//       : CompilationJob(CompilationJobKind::DynamicLink,
//                        CompilationJobLevel::Top, deps),
//         requiresLTO(requiresLTO) {}

// public:
//   //Command *ToCommand() const override { return this; }

// public:
//   static bool classof(const CompilationJob *job) {
//     return job->GetKind() == CompilationJobKind::DynamicLink;
//   }
// };

// class StaticLinkJob final : public CompilationJob {
// public:
//   StaticLinkJob(Inputs inputs)
//       : CompilationJob(CompilationJobKind::DynamicLink,
//                        CompilationJobLevel::Top, inputs) {}

//   StaticLinkJob(Deps deps)
//       : CompilationJob(CompilationJobKind::DynamicLink,
//                        CompilationJobLevel::Top, deps) {}

// public:
//   //Command *ToCommand() const override { return this; }

// public:
//   static bool classof(const CompilationJob *job) {
//     return job->GetKind() == CompilationJobKind::StaticLink;
//   }
// };

} // namespace stone
#endif
