#ifndef STONE_COMPILER_JOB_H
#define STONE_COMPILER_JOB_H

#include "stone/Basic/Color.h"
#include "stone/Basic/List.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Driver/Command.h"
#include "stone/Driver/CrashState.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/JobKind.h"

#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/Timer.h"

namespace stone {
class Tool;
class Job;
class TaskQueue;
class Compilation;

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

namespace job {
using Input = llvm::PointerUnion<stone::file::File *, Job *>;
using InputList = llvm::ArrayRef<job::Input>;
} // namespace job

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

class Job {
  friend TaskQueue;
  friend Compilation;

  JobKind kind;
  const Tool &tool;
  std::unique_ptr<JobStats> stats;
  file::Type outputFileType = file::Type::None;
  llvm::TinyPtrVector<job::Input> inputs;

  const char *GetNameByKind(JobKind kind) const;

public:
  using size_type = llvm::ArrayRef<job::Input>::size_type;
  using iterator = llvm::ArrayRef<job::Input>::iterator;
  using const_iterator = llvm::ArrayRef<job::Input>::const_iterator;

protected:
  Context &ctx;
  // Updated by the TaskQueue if or when the job is queued.
  JobID jobID = -1;
  void SetID(JobID jid) { jobID = jid; }

public:
  JobStage stage = JobStage::None;
  ThreadMode threaMode = ThreadMode::None;

public:
  Job() = delete;
  Job(JobKind kind, Context &ctx, const Tool &tool, job::InputList inputs,
      file::Type outputFileType);
  virtual ~Job();

public:
  JobID GetID() { return jobID; }
  const char *GetName() const { return Job::GetNameByKind(kind); }

  job::InputList GetInputs() { return inputs; }
  JobKind GetKind() const { return kind; }
  void AddInput(job::Input input) { inputs.push_back(input); }

public:
  /// Print a nice summary of this job
  virtual void Print(ColorfulStream &stream, CrashState *crashState = nullptr);

  /// Perform a complete dump of this job.
  virtual void Dump(ColorfulStream &stream, llvm::StringRef terminator = "\n",
                    CrashState *crashState = nullptr);

public:
  size_type size() const { return inputs.size(); }
  iterator begin() { return inputs.begin(); }
  iterator end() { return inputs.end(); }
  const_iterator begin() const { return inputs.begin(); }
  const_iterator end() const { return inputs.end(); }

public:
  // Required for llvm::dyn_cast
  static bool classof(const Job *job) {
    return (job->GetKind() >= JobKind::First &&
            job->GetKind() <= JobKind::Last);
  }
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
//   void Print(ColorfulStream &stream, CrashState *crashState = nullptr) override;

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
  llvm::SmallVector<job::Input, 16> forCompile;

  /// When are building the Jobs(s), keep track of the linker dependecies
  llvm::SmallVector<job::Input, 16> forLink;

  /// These are the top-level jobs -- we use them recursively to build
  llvm::SmallVector<job::Input, 16> forTop;

public:
  bool HasCompile() { return forCompile.size(); }
  void CacheForCompile(job::Input input) { forCompile.push_back(input); }

  // TODO: The approach that you are taking, this is not needed.
  bool HasLink() { return forLink.size(); }
  void CacheForLink(job::Input input) { forLink.push_back(input); }

  bool HasTop() { return forTop.size(); }
  void CacheForTop(job::Input input) { forTop.push_back(input); }

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
