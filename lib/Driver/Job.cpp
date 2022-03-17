#include "stone/Driver/Job.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using stone::Compilation;
using stone::Driver;
using stone::Job;
using stone::JobStats;

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

void JobStats::Print() {}

// Job::Job() : Job(ThreadingMode::Async) {}

// Job::Job(ThreadingMode threadingMode) : threadingMode(threadingMode) {}

// Job::~Job() {}

// void Job::Run() {}

void Driver::BuildJobs(Compilation &compilation, CompilationHotInfo &bcs) {}
