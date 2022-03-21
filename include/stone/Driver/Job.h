#ifndef STONE_COMPILER_JOB_H
#define STONE_COMPILER_JOB_H

#include "stone/Core/Color.h"
#include "stone/Core/List.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/Command.h"
#include "stone/Driver/CrashState.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/JobKind.h"
#include "stone/Driver/Request.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/Timer.h"

namespace stone {
class Tool;
class Job;
class JobQueue;
class Compilation;

class JobStats final : public Stats {
  const Job &job;

public:
  JobStats(const Job &job, Context &ctx)
      : Stats("job statistics:", ctx), job(job) {}
  void Print() override;
};

// The process ID
using JobID = int64_t;
enum class JobStage : uint8_t { None = 0, Running, Finished, Error };
enum class ThreadMode : uint8_t { None = 0, Sync, Async };

using InputList = llvm::ArrayRef<const file::File *>;
class Job : public Command {
  friend JobQueue;
  friend Compilation;

  JobKind kind;
  std::unique_ptr<JobStats> stats;
  file::Type outputFileType = file::Type::None;
  llvm::TinyPtrVector<const file::File *> inputs;

  const char *GetNameByKind(JobKind kind) const;

protected:
  Context &ctx;
  // Updated by the JobQueue if or when the job is queued.
  JobID jobID = -1;
  void SetID(JobID jid) { jobID = jid; }

public:
  JobStage stage = JobStage::None;
  ThreadMode threaMode = ThreadMode::None;

public:
  Job() = delete;
  Job(JobKind kind, Context &ctx, const Tool &tool, InputList inputs,
      file::Type outputFileType);
  virtual ~Job();

public:
  JobID GetID() { return jobID; }
  const char *GetName() const { return Job::GetNameByKind(kind); }

  llvm::ArrayRef<const file::File *> GetInputs() { return inputs; }
  JobKind GetKind() const { return kind; }
  void AddInput(const file::File *input) { inputs.push_back(input); }

public:
  /// Print a nice summary of this job
  virtual void Print(ColorOutputStream &stream,
                     CrashState *crashState = nullptr);

  /// Perform a complete dump of this job.
  virtual void Dump(ColorOutputStream &stream,
                    llvm::StringRef terminator = "\n",
                    CrashState *crashState = nullptr);

  // Required for llvm::dyn_cast
  static bool classof(const Job *job) {
    return (job->GetKind() >= JobKind::First &&
            job->GetKind() <= JobKind::Last);
  }
};

class CompileJob final : public Job {
public:
  CompileJob(Context &ctx, const Tool &tool, file::Type outputFileType)
      : Job(JobKind::Compile, ctx, tool, {}, outputFileType) {}

  CompileJob(Context &ctx, const Tool &tool, const file::File *input,
             file::Type outputFileType)
      : Job(JobKind::Compile, ctx, tool, input, outputFileType) {}

public:
  static bool classof(const Job *job) {
    return job->GetKind() == JobKind::Compile;
  }
};

// FlexJob can have dependencies
using DepList = llvm::ArrayRef<const Job *>;
class FlexJob : public Job {
  bool solo = false;
  llvm::TinyPtrVector<const Job *> deps;

public:
  using size_type = llvm::ArrayRef<const Job *>::size_type;
  using iterator = llvm::ArrayRef<const Job *>::iterator;
  using const_iterator = llvm::ArrayRef<const Job *>::const_iterator;

public:
  FlexJob(JobKind kind, Context &ctx, const Tool &tool, InputList inputs,
          file::Type outFileType)
      : Job(kind, ctx, tool, inputs, outFileType), solo(true), deps({}) {}

  FlexJob(JobKind kind, Context &ctx, const Tool &tool, DepList deps,
          file::Type outFileType)
      : Job(kind, ctx, tool, {}, outFileType), deps(deps), solo(false) {}

public:
  void AddDep(const Job *dep) {
    assert(solo);
    deps.push_back(dep);
  }

  bool IsSolo() { return (solo && (deps.size() == 0)); }

public:
  size_type size() const { return deps.size(); }
  iterator begin() { return deps.begin(); }
  iterator end() { return deps.end(); }
  const_iterator begin() const { return deps.begin(); }
  const_iterator end() const { return deps.end(); }

public:
  // Required for llvm::dyn_cast
  static bool classof(const Job *job) {
    return (job->GetKind() >= JobKind::First &&
            job->GetKind() <= JobKind::Last);
  }
};

class DynamicLinkJob final : public FlexJob {
  bool withLTO;

public:
  DynamicLinkJob(Context &ctx, const Tool &tool, InputList inputs,
                 bool withLTO = false)
      : FlexJob(JobKind::DynamicLink, ctx, tool, inputs, file::Type::Image),
        withLTO(withLTO) {}

  DynamicLinkJob(Context &ctx, const Tool &tool, DepList deps,
                 bool withLTO = false)
      : FlexJob(JobKind::DynamicLink, ctx, tool, deps, file::Type::Image),
        withLTO(withLTO) {}

  bool WithLTO() { return withLTO; }

public:
  static bool classof(const Job *job) {
    return job->GetKind() == JobKind::DynamicLink;
  }
};
class StaticLinkJob final : public FlexJob {
public:
  StaticLinkJob(Context &ctx, const Tool &tool, InputList inputs)
      : FlexJob(JobKind::StaticLink, ctx, tool, inputs, file::Type::Image) {}

  StaticLinkJob(Context &ctx, const Tool &tool, DepList deps)
      : FlexJob(JobKind::StaticLink, ctx, tool, deps, file::Type::Image) {}

public:
  static bool classof(const Job *job) {
    return job->GetKind() == JobKind::StaticLink;
  }
};

class ExecutableLinkJob final : public FlexJob {
public:
  ExecutableLinkJob(Context &ctx, const Tool &tool, InputList inputs)
      : FlexJob(JobKind::ExecutableLink, ctx, tool, inputs, file::Type::Image) {
  }

  ExecutableLinkJob(Context &ctx, const Tool &tool, DepList deps)
      : FlexJob(JobKind::ExecutableLink, ctx, tool, deps, file::Type::Image) {}

public:
  static bool classof(const Job *job) {
    return job->GetKind() == JobKind::ExecutableLink;
  }
};

class BatchJob : public Job {};

class Compilation;
class JobCache final {
public:
  /// We keep track of the jobs for the module that we are building.
  /// These are CompileJob
  llvm::SmallVector<const Job *, 16> forModule;

  /// When are building the Jobs(s), keep track of the linker dependecies
  llvm::SmallVector<const Job *, 16> forLink;

  /// These are the top-level jobs -- we use them recursively to build
  llvm::SmallVector<const Job *, 16> forTop;

public:
  bool ForModule() { return forModule.size(); }
  void CacheForModule(const Job *input) { forModule.push_back(input); }

  bool ForLink() { return forLink.size(); }
  void CacheForLink(const Job *input) { forLink.push_back(input); }

  bool ForTop() { return forTop.size(); }
  void CacheForTop(const Job *input) { forTop.push_back(input); }

public:
  void Finish(Compilation &compilation, const OutputOptions &outputOpts);
};

class ImageBaseName final {
  // llvm::Triple &triple;
  // llvm::SmallString<128> &buffer;
  llvm::StringRef baseInput;
  llvm::StringRef baseName;

public:
  ImageBaseName() {}
  ~ImageBaseName() = delete;
};

} // namespace stone

#endif
