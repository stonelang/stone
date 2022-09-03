#ifndef STONE_COMPILER_JOB_H
#define STONE_COMPILER_JOB_H

#include "stone/Basic/Color.h"
#include "stone/Basic/List.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Drive/Command.h"
#include "stone/Drive/CrashState.h"
#include "stone/Drive/DriverOptions.h"
#include "stone/Drive/JobKind.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

namespace stone {
class Tool;
class Job;
class Phase;
class TaskQueue;
class Compilation;
class ToolChain;
class LangContext;

class JobStats final : public Stats {
  const Job &job;

public:
  JobStats(const Job &job) : Stats("job statistics:"), job(job) {}
  void Print(ColorfulStream &stream) override;
};

// The process ID
using JobID = int64_t;
enum class JobStage : uint8_t { None = 0, Running, Finished, Error };
enum class ThreadMode : uint8_t { None = 0, Sync, Async };

class JobDetail final {
  llvm::StringRef execPath;
  llvm::ArrayRef<llvm::StringRef> args;
  llvm::ArrayRef<llvm::StringRef> env = llvm::None;

public:
  JobDetail() {}
  JobDetail(llvm::StringRef execPath, llvm::ArrayRef<llvm::StringRef> args,
            llvm::ArrayRef<llvm::StringRef> env)
      : execPath(execPath), args(args), env(env) {}

public:
  llvm::StringRef GetExecPath() { return execPath; }
  llvm::ArrayRef<llvm::StringRef> GetArgs() { return args; }
  llvm::ArrayRef<llvm::StringRef> GetEnv() { return env; }
};

enum class JobCondition {
  // There was no information about the previous build (i.e., an input map),
  // or the map marked this Job as dirty or needing a cascading build.
  // Be maximally conservative with dependencies.
  Always,
  // The input changed, or this job was scheduled as non-cascading in the last
  // build but didn't get to run.
  RunWithoutCascading,
  // The best case: input didn't change, output exists.
  // Only run if it depends on some other thing that changed.
  CheckDependencies,
  // Run no matter what (but may or may not cascade).
  NewlyAdded
};

class Job {

  friend TaskQueue;
  friend Compilation;

  std::unique_ptr<JobStats> stats;
  file::Type outputFileType = file::Type::None;

  llvm::SmallVector<const Job *> inputs;
  /// The phase which caused the creation of this Job, and the conditions
  /// under which it must be run.
  llvm::PointerIntPair<const Phase *, 2, JobCondition> phaseAndCondition;

protected:
  LangContext &ctx;
  // Updated by the TaskQueue if or when the job is queued.
  JobID jobID = -1;
  void SetID(JobID jid) { jobID = jid; }

public:
  JobStage stage = JobStage::None;
  ThreadMode threaMode = ThreadMode::None;

public:
  Job() = delete;
  Job(const Phase &phase, LangContext &ctx,
      llvm::SmallVectorImpl<const Job *> &&inputs, file::Type outputFileType);
  virtual ~Job();

public:
  JobID GetID() { return jobID; }
  llvm::ArrayRef<const Job *> GetInputs() { return inputs; }

  // TODO: Think about
  void AddInput(const Job *input) { inputs.push_back(input); }

  const Phase &GetPhase() const { return *phaseAndCondition.getPointer(); }
  JobCondition GetJobCondition() const { return phaseAndCondition.getInt(); }
  void SetJobCondition(JobCondition jc) { phaseAndCondition.setInt(jc); }

public:
  /// Print a nice summary of this job
  virtual void Print(ColorfulStream &stream, CrashState *crashState = nullptr);

  /// Perform a complete dump of this job.
  virtual void Dump(ColorfulStream &stream, llvm::StringRef terminator = "\n",
                    CrashState *crashState = nullptr);
};

// class CompileJob final : public Job {
//   job::Input primaryInput;

// public:
//   CompileJob(Context &ctx, const Tool &tool, file::Type outputFileType);

//   CompileJob(Context &ctx, const Tool &tool, job::Input input,
//              file::Type outputFileType);

// public:
//   job::Input GetPrimaryInput() { return primaryInput; }
//   // void SetPrimaryInput(job::Input input) { primaryInput = input; }

//   /// Print a nice summary of this job
//   void Print(ColorfulStream &stream, CrashState *crashState = nullptr)
//   override;

//   /// Perform a complete dump of this job.
//   void Dump(ColorfulStream &stream, llvm::StringRef terminator = "\n",
//             CrashState *crashState = nullptr) override;

// public:
//   static bool classof(const Job *job) {
//     return job->GetKind() == JobKind::Compile;
//   }
// };

// class DynamicLinkJob final : public Job {
//   bool withLTO;

// public:
//   DynamicLinkJob(Context &ctx, const Tool &tool, job::InputList inputs,
//                  bool withLTO = false)
//       : Job(JobKind::DynamicLink, ctx, tool, inputs, file::Type::Image),
//         withLTO(withLTO) {}

//   bool WithLTO() { return withLTO; }

// public:
//   static bool classof(const Job *job) {
//     return job->GetKind() == JobKind::DynamicLink;
//   }
// };
// class StaticLinkJob final : public Job {
// public:
//   StaticLinkJob(Context &ctx, const Tool &tool, job::InputList inputs)
//       : Job(JobKind::StaticLink, ctx, tool, inputs, file::Type::Image) {}

// public:
//   static bool classof(const Job *job) {
//     return job->GetKind() == JobKind::StaticLink;
//   }
// };

// class ExecutableLinkJob final : public Job {
// public:
//   ExecutableLinkJob(Context &ctx, const Tool &tool, job::InputList inputs)
//       : Job(JobKind::ExecutableLink, ctx, tool, inputs, file::Type::Image) {}

// public:
//   static bool classof(const Job *job) {
//     return job->GetKind() == JobKind::ExecutableLink;
//   }
// };

// class CPUJob final : public Job {
// public:
// };

class Compilation;
class JobCache final {
public:
  /// We keep track of the jobs for the module that we are building.
  /// These are CompileJob
  llvm::SmallVector<const Job *, 16> forCompile;

  /// When are building the Jobs(s), keep track of the linker dependecies
  llvm::SmallVector<const Job *, 16> forLink;

  /// These are the top-level jobs -- we use them recursively to build
  llvm::SmallVector<const Job *, 16> forTopLevel;

  // llvm::DenseMap<std::pair<const PhaseInput *, const ToolChain *>,
  // job::Input *>;

public:
  bool HasCompile() { return forCompile.size(); }
  void CacheForCompile(const Job *input) { forCompile.push_back(input); }

  // TODO: The approach that you are taking, this is not needed.
  bool HasLink() { return forLink.size(); }
  void CacheForLink(const Job *input) { forLink.push_back(input); }

  bool HasTopLevel() { return forTopLevel.size(); }
  void CacheForTopLevel(const Job *input) { forTopLevel.push_back(input); }

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

/// A map for caching Jobs for a given Phase/ToolChain pair
using JobCacheMap =
    llvm::DenseMap<std::pair<const Phase *, const ToolChain *>, Job *>;

} // namespace stone

#endif
