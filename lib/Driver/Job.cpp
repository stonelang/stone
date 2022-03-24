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
    : tool(tool), kind(kind), ctx(ctx), inputs(inputs),
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

static void BuildLinkJob(Compilation &compilation, JobCache &jc,
                         const OutputOptions &outputOpts) {

  auto &toolChain = compilation.GetToolChain();
  auto &driver = compilation.GetDriver();

  // if (outputOptions.CanLink()) {

  // }
  // toolChain.ConstructCompileJob()
  //       hc.currentRequest =
  //           compilation.GetDriver().MakeIntent<CompileJobRequest>(
  //               hc.currentRequest,
  //               compilation.GetDriver().GetOutputFileType());
  //       hc.AddModuleInput(hc.currentRequest);
  //       if (outputOptions.CanLink()) {
  //         hc.AddLinkInput(hc.currentRequest);
  //       }

  // if (outputOptions.CanLink()) {
  //        hc.AddLinkInput(hc.currentRequest);
  //        break;
  //      }
}

static void BuildCompileJob(Compilation &compilation, const file::File &input,
                            JobCache &jc, const OutputOptions &outputOpts) {

  auto &toolChain = compilation.GetToolChain();
  // auto &driver = compilation.GetDriver();
  // auto job = toolChain.ConstructCompileJob(compilation, input, outputOpts);

  // TODO: Maybe move into transitionQ -> finalQ
  // if(compileJob){
  //   compilation.EnqueueJob(compileJob);
  // }

  // jc.CacheForModule(job);
  // // Cache for now:
  // if (outputOpts.CanLink()) {
  //   jc.CacheForLink(job);
  // }
}

static void BuildCompilationModeMultiple(Compilation &compilation,
                                         const file::Files &inputs,
                                         JobCache &jc,
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
      case file::Type::Object: {
        // TODO: Cannot think of a scenario now where we have
        // an object file and we are not linking.
        assert(outputOpts.CanLink());
        jc.CacheForLink(const_cast<file::File *>(&input));
        break;
      }
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
  // assert(!inputs.empty());

  switch (GetCompilationMode()) {
  case CompilationMode::Quadratic:
    BuildCompilationModeMultiple(compilation, inputs, hc.GetJobCache(),
                                 outputOpts);
    break;
  // case CompilationMode::Single:
  //   BuildSingleCompilingModel(compilation, hc, inputs, outputOptions);
  //   break;
  // case CompilationMode::CPU:
  //   BuildBatchCompilingModel(compilation, hc, inputs, outputOptions);
  //   break;
  default:
    stone::Panic("Unsupported Compiling mode");
  }

  if (outputOpts.CanLink() && hc.GetJobCache().ForLink()) {
  }
  // TryBuildLinkJob();

  // // First, check to see if there are any top-level requests
  // if (hc.GetJobCache().ForTop()) {
  //   // We are building the jobs recursively and we are linking,
  //   module-merging
  //   // and the like.

  // } else {
  //   // This must be a compile only scenario
  //   assert(JustCompile());
  // }
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
                      const OutputOptions &outputOpts) {
  auto &driver = compilation.GetDriver();

  if ((forLink.size() > 0) && outputOpts.CanLink()) {

    switch (driver.GetLinkMode()) {
    case LinkMode::EmitExecutable: {
      break;
    }
    case LinkMode::EmitDynamicLibrary: {
      break;
    }
    case LinkMode::EmitStaticLibrary: {
      break;
    }
    default:
      stone::Panic("Invalid linking mode");
    }
  }
}

void JobStats::Print() {}
