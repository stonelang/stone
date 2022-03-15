#ifndef STONE_COMPILER_JOB_H
#define STONE_COMPILER_JOB_H

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
class Job;
class Tool;

// The process ID
using JobID = int64_t;
class Driver;

class JobStats final : public Stats {
  const Job &job;

public:
  JobStats(const Job &job, Context &ctx)
      : Stats("job statistics:", ctx), job(job) {}
  void Print() override;
};

// TODO: JobStatus
enum class JobStage : uint8_t { None = 0, Running, Finished, Error };
enum class ThreadingMode : uint8_t { None = 0, Sync, Async };

// class CommandIntentExecutor final : public IntentExecutor {
//   Intent &intent;
//   CommandInvocation &ci;

// public:
//   CommandIntentExecutor(Intent &intent, CommandInvocation &ci)
//       : intent(intent), ci(ci) {}

// public:
//   void RunAsync() override;
//   void RunSync() override;
// };

class Job final {

  friend class JobQueue;
  IntentExecutor &executor;
  ThreadingMode threadingMode = ThreadingMode::None;

protected:
  // Updated by the JobQueue if or when the job is queued.
  int queueID = -1;

public:
  Job() = delete;

public:
  Job(IntentExecutor &executor);
  Job(IntentExecutor &executor, ThreadingMode threadingMode);
  virtual ~Job();

public:
  virtual void Run();

public:
  IntentExecutor &GetExecutor() { return executor; }
  ThreadingMode GetThreadingMode() { return threadingMode; }
};

// class Job : public Command {
//   friend JobStats;
//   friend class JobQueue;

// protected:
//   JobID jobID;

//   bool isAsync;
//   JobKind jobKind;
//   Context &ctx;
//   file::Files inputs;
//   stone::ConstList<Job> deps;
//   std::unique_ptr<JobStats> stats;
//   int queueID = -1;

//   Intent &intent;

// public:
//   JobStage stage = JobStage::None;

// public:
//   Job(Intent &intent, Context &ctx, Tool &tool);
//   virtual ~Job();

// public:
//   JobKind GetKind() const { return jobKind; }
//   const ConstList<Job> &GetDeps() const { return deps; }

//   Intent &GetIntent() const { return intent; }

//   void AddInput(const file::File input);
//   void AddDep(const Job *job);

//   Context &GetContext() { return ctx; }
//   bool IsAsync() { return isAsync; }

//   void Print(const char *terminator, bool quote, CrashState *crashState)
//   const; const char *GetName() const { return Job::GetNameByKind(jobKind); }

//   void SetJobID(JobID jid) { jobID = jid; }
//   JobID GetJobID() { return jobID; }

//   int GetQueueID() const { return queueID; }
//   virtual void PrintIntent() = 0;
//   virtual int ExecuteAsync();
//   virtual int ExecuteSync();

// protected:
//   stone::ColorOutputStream &OS();

// public:
//   static const char *GetNameByKind(JobKind jobKind);
// };

// class CompileJob final : public Job {
// public:
//   CompileJob(Intent &intent, Context &ctx, Tool &tool);

//   int ExecuteAsync() override;
//   int ExecuteSync() override;
//   void PrintIntent() override;

// public:
//   static bool classof(const Job *j) {
//     return j->GetIntent().GetKind() == IntentKind::Compile;
//   }
// };

// class LinkJob : public Job {
//   LinkKind linkKind;
//   bool requiresLTO;

// public:
//   // Some jobs only consume inputs -- For example, LinkJob
//   LinkJob(Intent &intent, Context &ctx, Tool &tool, bool requiresLTO,
//           LinkKind linkKind);

//   void PrintIntent() override;

// public:
//   LinkKind GetLinkKind() { return linkKind; }
//   bool RequiresLTO() { return requiresLTO; }
// };

// class DynamicLinkJob final : public LinkJob {
//   // Only Dynamic requirest LTO
// public:
//   DynamicLinkJob(Intent &intent, Context &ctx, Tool &tool, bool requiresLTO);

//   int ExecuteAsync() override;
//   int ExecuteSync() override;

// public:
//   static bool classof(const Job *j) {
//     return j->GetIntent().GetKind() == IntentKind::DynamicLink;
//   }
// };

// class StaticLinkJob final : public LinkJob {
// public:
//   StaticLinkJob(Intent &intent, Context &ctx, Tool &tool, bool requiresLTO);
//   int ExecuteAsync() override;
//   int ExecuteSync() override;

// public:
//   static bool classof(const Job *j) {
//     return j->GetIntent().GetKind() == IntentKind::StaticLink;
//   }
// };

// class ExecutableLinkJob final : public LinkJob {
// public:
//   ExecutableLinkJob(Intent &intent, Context &ctx, Tool &tool, bool
//   requiresLTO);
//   // void BuildCmdOutput() override;

//   int ExecuteAsync() override;
//   int ExecuteSync() override;

// public:
//   static bool classof(const Job *j) {
//     return j->GetIntent().GetKind() == IntentKind::ExecutableLink;
//   }
// };

class ImageBaseName final {
  // llvm::Triple &triple;
  // llvm::SmallString<128> &buffer;
  llvm::StringRef baseInput;
  llvm::StringRef baseName;

public:
  ImageBaseName() {}
  ~ImageBaseName() = delete;
};

class BuiltJobs final {

public:
  BuiltJobs(const BuiltJobs &) = delete;
  void operator=(const BuiltJobs &) = delete;

public:
  BuiltJobs();
  ~BuiltJobs();

private:
  void Finish();
};

} // namespace stone

#endif
