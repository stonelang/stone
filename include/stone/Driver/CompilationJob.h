#ifndef STONE_COMPILATIONJOB_H
#define STONE_COMPILATIONJOB_H

#include "stone/Core/Color.h"
#include "stone/Core/Context.h"
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

enum class CompilationJobKind : uint8_t {
  None = 0,
  Compile,
  DynamicLink,
  StaticLink,
  ExecLink,
  First = Compile,
  Last = ExecLink,
};

enum class CompilationJobStage : uint8_t { None = 0, Running, Finished, Error };

/// See OutputOptions
enum class ThreadingKind : uint8_t { None = 0, Single, Multi };

using Inputs = llvm::ArrayRef<const file::File *>;
using Deps = llvm::ArrayRef<const CompilationJob *>;

class CompilationJob : public Command {

  friend class Compilation;

  Context &ctx;
  CompilationJobID jobID;
  CompilationJobKind kind;
  llvm::TinyPtrVector<const file::File *> inputs;

protected:
  int queueID = -1;

public:
  CompilationJobStage stage = CompilationJobStage::None;

public:
  CompilationJob(CompilationJobKind kind, Context &ctx, const Tool &tool,
                 Inputs inputs)
      : Command(tool), kind(kind), ctx(ctx), inputs(inputs) {}

public:
  virtual void Run() {}

  /// Print a nice summary of this job
  void Print(ColorOutputStream &stream);

  /// Perform a complete dump of this job.
  void Dump(ColorOutputStream &stream, llvm::StringRef terminator = "\n");

  int GetQueueID() const { return queueID; }
  ColorOutputStream &OS() { return ctx.Out(); }

protected:
  virtual const Command *ToCommand() const {
    assert(false && "Not implemented on purpose.");
    return nullptr;
  }

public:
  llvm::ArrayRef<const file::File *> GetInputs() { return inputs; }
  CompilationJobKind GetKind() const { return kind; }
  void AddInput(const file::File *input) { inputs.push_back(input); }

public:
  // Required for llvm::dyn_cast
  static bool classof(const CompilationJob *job) {
    return (job->GetKind() >= CompilationJobKind::First &&
            job->GetKind() <= CompilationJobKind::Last);
  }
};

class CompileJob final : public CompilationJob {
public:
  CompileJob(Context &ctx, const Tool &tool, const file::File *input)
      : CompilationJob(CompilationJobKind::Compile, ctx, tool, input) {}

public:
  void Run() override;

protected:
  const Command *ToCommand() const override;

public:
  static bool classof(const CompilationJob *job) {
    return job->GetKind() == CompilationJobKind::Compile;
  }
};

// These jobs have no parents.
class TopLevelJob : public CompilationJob {

  llvm::TinyPtrVector<const CompilationJob *> deps;

public:
  using size_type = llvm::ArrayRef<const CompilationJob *>::size_type;
  using iterator = llvm::ArrayRef<const CompilationJob *>::iterator;
  using const_iterator = llvm::ArrayRef<const CompilationJob *>::const_iterator;

public:
  TopLevelJob(CompilationJobKind kind, Context &ctx, const Tool &tool,
              Inputs inputs)
      : CompilationJob(kind, ctx, tool, inputs) {}

  TopLevelJob(CompilationJobKind kind, Context &ctx, const Tool &tool,
              Deps deps)
      : CompilationJob(kind, ctx, tool, {}), deps(deps) {}

public:
  virtual void Run() {}
  void AddDep(const CompilationJob *dep) { deps.push_back(dep); }

protected:
  const Command *ToCommand() const override {
    assert(false && "Not implemented on purpose.");
    return nullptr;
  }

public:
  size_type size() const { return deps.size(); }
  iterator begin() { return deps.begin(); }
  iterator end() { return deps.end(); }
  const_iterator begin() const { return deps.begin(); }
  const_iterator end() const { return deps.end(); }

public:
  // Required for llvm::dyn_cast
  static bool classof(const CompilationJob *job) {
    return (job->GetKind() >= CompilationJobKind::First &&
            job->GetKind() <= CompilationJobKind::Last);
  }
};

class DynamicLinkJob final : public TopLevelJob {
  bool requiresLTO;

public:
  DynamicLinkJob(Context &ctx, const Tool &tool, Inputs inputs,
                 bool requiresLTO = false)
      : TopLevelJob(CompilationJobKind::DynamicLink, ctx, tool, inputs),
        requiresLTO(requiresLTO) {}

  DynamicLinkJob(Context &ctx, const Tool &tool, Deps deps,
                 bool requiresLTO = false)
      : TopLevelJob(CompilationJobKind::DynamicLink, ctx, tool, deps),
        requiresLTO(requiresLTO) {}

public:
  void Run() override;

protected:
  const Command *ToCommand() const override;

public:
  static bool classof(const CompilationJob *job) {
    return job->GetKind() == CompilationJobKind::DynamicLink;
  }
};

class StaticLinkJob final : public TopLevelJob {

public:
  StaticLinkJob(Context &ctx, const Tool &tool, Inputs inputs)
      : TopLevelJob(CompilationJobKind::StaticLink, ctx, tool, inputs) {}

  StaticLinkJob(Context &ctx, const Tool &tool, Deps deps)
      : TopLevelJob(CompilationJobKind::StaticLink, ctx, tool, deps) {}

public:
  void Run() override;

protected:
  const Command *ToCommand() const override;

public:
  static bool classof(const CompilationJob *job) {
    return job->GetKind() == CompilationJobKind::StaticLink;
  }
};

class ExecLinkJob final : public TopLevelJob {

public:
  ExecLinkJob(Context &ctx, const Tool &tool, Inputs inputs)
      : TopLevelJob(CompilationJobKind::ExecLink, ctx, tool, inputs) {}

  ExecLinkJob(Context &ctx, const Tool &tool, Deps deps)
      : TopLevelJob(CompilationJobKind::ExecLink, ctx, tool, deps) {}

public:
  void Run() override;

protected:
  const Command *ToCommand() const override;

public:
  static bool classof(const CompilationJob *job) {
    return job->GetKind() == CompilationJobKind::ExecLink;
  }
};

} // namespace stone
#endif
