#ifndef STONE_COMPILER_JOB_H
#define STONE_COMPILER_JOB_H

#include "stone/Core/Color.h"
#include "stone/Core/List.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/Command.h"
#include "stone/Driver/CrashState.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/JobKind.h"
#include "stone/Driver/JobRequest.h"

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

class Job {
  friend class JobQueue;

  JobRequest &request;
  const JobInvocation &invocation;
  /// The list of other Jobs which are inputs to this Job.
  llvm::SmallVector<const Job *, 4> deps;

protected:
  // Updated by the JobQueue if or when the job is queued.
  int queueID = -1;

public:
  JobStage stage = JobStage::None;
  /// public for now
  ThreadingMode threadingMode = ThreadingMode::None;

public:
  Job() = delete;

public:
  Job(JobRequest &request, llvm::SmallVectorImpl<const Job *> &&deps,
      const JobInvocation &invocation);

  virtual ~Job();

public:
  /// Print a nice summary of this job
  virtual void Print(ColorOutputStream &stream,
                     CrashState *crashState = nullptr);

  /// Perform a complete dump of this job.
  virtual void Dump(ColorOutputStream &stream,
                    llvm::StringRef terminator = "\n",
                    CrashState *crashState = nullptr);

public:
  int GetQueueID() const { return queueID; }
  stone::ColorOutputStream &OS();
  JobRequest &GetJobRequest() { return request; }
  const JobInvocation &GetJobInvocation() const { return invocation; }
};

class BatchJob : public Job {};

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
//   LinkMode linkMode;
//   bool requiresLTO;

// public:
//   // Some jobs only consume inputs -- For example, LinkJob
//   LinkJob(Intent &intent, Context &ctx, Tool &tool, bool requiresLTO,
//           LinkMode linkMode);

//   void PrintIntent() override;

// public:
//   LinkMode GetLinkMode() { return linkMode; }
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
//

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

} // namespace stone

#endif
