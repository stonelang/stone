#include "stone/Driver/Job.h"
#include "stone/Core/Defer.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

const char *Job::GetNameByKind(JobKind jobKind) const {
  switch (jobKind) {
  case JobKind::Compile:
    return "compile";
  case JobKind::Backend:
    return "backend";
  case JobKind::Assemble:
    return "assemble";
  case JobKind::DynamicLink:
    return "dynamic-link";
  case JobKind::StaticLink:
    return "static-link";
  case JobKind::ExecutableLink:
    return "executable-link";
  default:
    stone::Panic("Invalid JobKind");
  }
}

Job::Job(JobKind kind, Context &ctx, const Tool &tool, job::InputList inputs,
         file::Type outputFileType)
    : Command(tool), kind(kind), ctx(ctx), inputs(inputs),
      outputFileType(outputFileType) {
  stats = std::make_unique<JobStats>(*this, ctx);
  ctx.GetStatEngine().Register(stats.get());
}
Job::~Job() {}

/// -print-jobs
void Job::Print(ColorOutputStream &stream, CrashState *crashState) {
  // stream() << std::to_string(GetQueueID()) << ":";
  // stream().UseGreen();
  // stream() << GetName();
  // stream().Reset();
  // stream() << "(";
  // int inputSize = inputs.size();
  // for (auto &input : inputs) {
  //   --inputSize;
  //   stream() << input.GetName();
  //   if (inputSize != 0) {
  //     stream() << ",";
  //   }
  // }
  // stream() << ")"
  //          << " -> "
  //          << "object" << '\n';
  // stream() << '\n';
}

// -print-jobs -v
void Job::Dump(ColorOutputStream &stream, llvm::StringRef terminator,
               CrashState *crashState) {
  //
}

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

static void BuildCompileJob(Compilation &compilation, const file::File &input,
                            JobCache &jc, const OutputOptions &outputOpts) {

  auto &toolChain = compilation.GetToolChain();
  auto &driver = compilation.GetDriver();
  auto compileJob =
      toolChain.ConstructCompileJob(compilation, input, outputOpts);

  // TODO: Maybe move into transitionQ -> finalQ
  // if(compileJob){
  //   compilation.EnqueueJob(compileJob);
  // }
  // Cache for now:
  jc.CacheForLink(compileJob);
}

static void BuildMultipleCompilingModel(Compilation &compilation,
                                        const file::Files &inputs, JobCache &jc,
                                        const OutputOptions &outputOpts) {

  auto &toolChain = compilation.GetToolChain();
  auto &driver = compilation.GetDriver();

  for (auto &input : inputs) {
    // TODO: Way out there, but there is potential for git here?
    if (driver.GetBuildSystem().IsDirty(input)) {

      assert(input.GetType() == driver.GetInputFileType() &&
             "Incompatible input file types");
      assert(file::IsPartOfCompilation(input.GetType()));

      switch (input.GetType()) {
      case file::Type::Stone: {
        BuildCompileJob(compilation, input, jc, outputOpts);
        break;
      }
      case file::Type::Object:

        break;
      default:
        stone::Panic("Alien file -- cannot build job.");
      }
    }
  }
}

void Driver::BuildJobs(Compilation &compilation, HotCache &hc,
                       const file::Files &inputs,
                       const OutputOptions &outputOpts) {

  STONE_DEFER { hc.GetJobCache().Finish(compilation, outputOpts); };

  // We assert here because this should have been checked above.
  assert(inputs.empty());

  switch (GetCompilingModelKind()) {
  case CompilingModelKind::Multiple:
    BuildMultipleCompilingModel(compilation, inputs, hc.GetJobCache(),
                                outputOpts);
    break;
  // case CompilingModelKind::Single:
  //   BuildSingleCompilingModel(compilation, hc, inputs, outputOptions);
  //   break;
  // case CompilingModelKind::Batch:
  //   BuildBatchCompilingModel(compilation, hc, inputs, outputOptions);
  //   break;
  default:
    stone::Panic("Unsupported Compiling mode");
  }

  // First, check to see if there are any top-level requests
  if (hc.GetReqCache().ForTop()) {
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

void JobCache::Finish(Compilation &compilation,
                      const OutputOptions &outputOpts) {}
void JobStats::Print() {}
