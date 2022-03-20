#include "stone/Driver/Job.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/Intent.h"

using stone::Compilation;
using stone::CompilationIntent;
using stone::Driver;
using stone::Intent;
using stone::Job;
using stone::JobStats;

Job::Job(const JobInvocation &invocation) : invocation(invocation) {}

Job::Job(const JobInvocation &invocation,
         llvm::SmallVectorImpl<const Job *> &&deps)
    : invocation(invocation), deps(std::move(deps)) {}

Job::~Job() {}

// Job::Job(Intent &intent, Context &ctx, Tool &tool, ThreadingMode
// threadingMode)
//     : Command(tool), intent(intent), ctx(ctx), jobKind(jobKind),
//     jobID(0),
//       isAsync(true) {

//   stats = std::make_unique<JobStats>(*this, ctx);
//   ctx.GetStatEngine().Register(stats.get());
// }

// void Job::AddDep(const Job *job) { deps.Add(job); }
// void Job::AddInput(const file::File input) { inputs.push_back(input); }

// Job::~Job() {}

// stone::ColorOutputStream &Job::OS() { return ctx.Out(); }

// int Job::ExecuteAsync() {
//   // TODO: Think about putting this outside
//   stage = JobStage::Running;

//   // OS() << "Running job(async) :" << GetName()
//   //      << "with tool:" << GetTool().GetFullName() << '\n';
//   for (auto &dep : deps) {
//     assert(dep.stage == JobStage::Finished &&
//            "dependecy jobs did not all finish");
//   }

//   // Build up the arguments
//   for (auto &input : inputs) {
//     args.push_back(input.GetName());
//   }

//   cmd::ExecuteAsync(*this, &ctx);
//   stage = JobStage::Finished;

//   // OS() << "Finished job(async) :" << GetName() << '\n';

//   return 0;
// }
// int Job::ExecuteSync() {
//   OS() << "Running job(sync) :" << GetName() << '\n';

//   cmd::ExecuteSync(*this, &ctx);

//   return 0;
// }

// void Job::Print(const char *terminator, bool quote, CrashState *crash) const
// {}

// const char *Job::GetNameByKind(JobKind jobKind) {
//   switch (jobKind) {
//   case JobKind::Compile:
//     return "compile";
//   case JobKind::Backend:
//     return "backend";
//   case JobKind::Assemble:
//     return "assemble";
//   case JobKind::DynamicLink:
//     return "dynamic-link";
//   case JobKind::StaticLink:
//     return "static-link";
//   case JobKind::ExecutableLink:
//     return "executable-link";
//   default:
//     assert(false && "Invalid JobKind");
//   }
// }

/// Print a nice summary of this job
void Job::Print(ColorOutputStream &stream, CrashState *crashState) {}

/// Perform a complete dump of this job.
void Job::Dump(ColorOutputStream &stream, llvm::StringRef terminator,
               CrashState *crashState) {}

void JobStats::Print() {}

// Job::Job() : Job(ThreadingMode::Async) {}

// Job::Job(ThreadingMode threadingMode) : threadingMode(threadingMode) {}

// Job::~Job() {}

// void Job::Run() {}

// static void BuildJobsForTopLevelIntent(Compilation &C,
//                                        const CompilationIntent *ci) {

//   for (const Intent *input : *ci) {
//     if (auto *processIntent = llvm::dyn_cast<CompilationIntent>(input)) {
//     }
//   }
// }

void Driver::BuildJobs(Compilation &compilation, const HotCache &hc,
                       const OutputOptions &outputOpts) {

  // switch (GetCompilingModelKind()) {
  // case CompilingModelKind::Multiple:
  //   BuildMultipleCompilingModel(comp, hc, inputs, outputOptions);
  //   break;
  // case CompilingModelKind::Single:
  //   BuildSingleCompilingModel(comp, hc, inputs, outputOptions);
  //   break;
  // case CompilingModelKind::Batch:
  //   BuildBatchCompilingModel(comp, hc, inputs, outputOptions);
  //   break;
  // default:
  //   stone::Panic("Unsupported Compiling mode");
  // }

  // First, check to see if there are any top-level requests
  if (hc.HasTopLevelRequest()) {
    // We are building the jobs recursively and we are linking, module-merging
    // and the like.

  } else {
    // This must be a compile only scenario
    assert(JustCompile());
  }

  //   for (const Intent *intent : chi.topLevelIntents) {
  //     if (auto *ci = llvm::dyn_cast<CompilationIntent>(intent)) {

  //       assert(ci->GetLevel() == IntentLevel::Top);
  //       BuildJobsForTopLevelIntent(compilation, ci);
  //     }
  //   }
}

// int job::RunSync(const Command &c, Context *ctx) {

//   assert(c.waitSecs > 0 && "Wait seconds must be greater than 0");
//   return llvm::sys::ExecuteAndWait(
//       c.GetTool().GetFullName(), llvm::ArrayRef<llvm::StringRef>(c.args),
//       c.env, c.redirects, c.waitSecs, c.memLimit, c.errMsg, c.failed);
// }
// int job::RunAsync(const Command &c, Context *ctx) {

//   assert(c.waitSecs == 0 && "Wait seconds must be equal to 0");
//   return llvm::sys::ExecuteAndWait(
//       c.GetTool().GetFullName(), llvm::ArrayRef<llvm::StringRef>(c.args),
//       c.env, c.redirects, c.waitSecs, c.memLimit, c.errMsg, c.failed);
// }
