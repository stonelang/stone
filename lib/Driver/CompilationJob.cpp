#include "stone/Driver/CompilationJob.h"
#include "stone/Core/Defer.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

/// -print-jobs
void CompilationJob::Print(ColorOutputStream &stream, CrashState *crashState) {

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

/// -print-jobs -v
void CompilationJob::Dump(ColorOutputStream &stream, llvm::StringRef terminator,
                          CrashState *crashState) {

  //
}
// void CompileJob::Run() { cmd::ExecuteAsync(*ToCommand()); }

// const Command *CompileJob::ToCommand() const {
//   // TODO: Build out command

//   return llvm::dyn_cast<Command>(this);
// }

/// -print-jobs
void UniversalJob::Print(ColorOutputStream &stream, CrashState *crashState) {}

/// -print-jobs -v
void UniversalJob::Dump(ColorOutputStream &stream, llvm::StringRef terminator,
                        CrashState *crashState) {

  for (auto dep : *this) {
  }
}

// void DynamicLinkJob::Run() { cmd::ExecuteAsync(*ToCommand()); }

// const Command *DynamicLinkJob::ToCommand() const {
//   // TODO: Build out command

//   return llvm::dyn_cast<Command>(this);
// }

static void BuildLinkJob(Compilation &compilation, const file::File &input,
                         JobCache &jc, const OutputOptions &outputOptions) {

  auto &toolChain = compilation.GetToolChain();
  auto &driver = compilation.GetDriver();

  // toolChain.ConstructCompileJob()
  //       hc.currentRequest =
  //           compilation.GetDriver().MakeRequest<CompileJobRequest>(
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
                            JobCache &jc, const OutputOptions &outputOptions) {

  auto &toolChain = compilation.GetToolChain();
  auto &driver = compilation.GetDriver();

  // toolChain.ConstructCompileJob()
  //       hc.currentRequest =
  //           compilation.GetDriver().MakeRequest<CompileJobRequest>(
  //               hc.currentRequest,
  //               compilation.GetDriver().GetOutputFileType());
  //       hc.AddModuleInput(hc.currentRequest);
  //       if (outputOptions.CanLink()) {
  //         hc.AddLinkInput(hc.currentRequest);
  //       }
}
static void BuildBatchCompilingModelJobs(Compilation &compilation,
                                         const file::Files &inputs,
                                         JobCache &jc,
                                         const OutputOptions &outputOptions) {}

static void BuildSingleCompilingModelJobs(Compilation &compilation,
                                          const file::Files &inputs,
                                          JobCache &jc,
                                          const OutputOptions &outputOptions) {

  auto &toolChain = compilation.GetToolChain();
  auto &driver = compilation.GetDriver();

  // // Create a single CompileJobRequest to handl all InputRequest(s)
  // auto *compileRequest = compilation.GetDriver().MakeRequest<CompileJobRequest>(
  //     compilation.GetDriver().GetOutputFileType());
  // for (auto &input : inputs) {

  //   if (driver.GetBuildSystem().IsDirty(input)) {
  //     assert(input.GetType() == driver.GetInputFileType() &&
  //            "Incompatible input file types");

  //     assert(file::IsPartOfCompilation(input.GetType()));

  //     // compileRequest->AddInput(.MakeRequest<InputRequest>(input));

  //     // hc.AddModuleInput(compileRequest);
  //     // if (outputOptions.CanLink()) {
  //     //   hc.AddLinkInput(hc.currentRequest);
  //     // }
  //   }
  // }
}

static void
BuildMultipleCompilingModelJobs(Compilation &compilation,
                                const file::Files &inputs, JobCache &jc,
                                const OutputOptions &outputOptions) {

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
        BuildCompileJob(compilation, input, jc, outputOptions);
        break;
      }
      case file::Type::Object:
        BuildCompileJob(compilation, input, jc, outputOptions);
        break;
      default:
        stone::Panic("Alien file -- cannot build job.");
      }
    }
  }
}

void Driver::BuildCompilationJobs(Compilation &compilation,
                                  const file::Files &inputs,
                                  const OutputOptions &outputOptions) {

  JobCache jc;
  STONE_DEFER { jc.Finish(compilation, outputOptions); };

  // We assert here because this should have been checked above.
  assert(inputs.empty());

  switch (GetCompilingModelKind()) {
  case CompilingModelKind::Multiple:
    BuildMultipleCompilingModelJobs(compilation, inputs, jc, outputOptions);
    break;
  case CompilingModelKind::Single:
    BuildSingleCompilingModelJobs(compilation, inputs, jc, outputOptions);
    break;
  case CompilingModelKind::Batch:
    BuildBatchCompilingModelJobs(compilation, inputs, jc, outputOptions);
    break;
  default:
    stone::Panic("Unsupported Compiling mode");
  }
}
// Now, we process all of the jobs that are top level
void JobCache::Finish(Compilation &compilation,
                      const OutputOptions &outputOpts) {
  // auto &driver = compilation.GetDriver();

  // if ((forLink.size() > 0) && outputOptions.CanLink()) {

  //   switch (driver.GetLinkMode()) {
  //   case LinkMode::EmitExecutable: {
  //     break;
  //   }
  //   case LinkMode::EmitDynamicLibrary: {
  //     break;
  //   }
  //   case LinkMode::EmitStaticLibrary: {
  //     break;
  //   }
  //   default:
  //     stone::Panic("Invalid linking mode");
  //   }
  // }
}