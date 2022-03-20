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

static void BuildBatchCompilingModelJobs(Compilation &compilation,
                                         const file::Files &inputs,
                                         JobCache &jc,
                                         const OutputOptions &outputOptions) {}

static void BuildSingleCompilingModelJobs(Compilation &compilation,
                                          const file::Files &inputs,
                                          JobCache &jc,
                                          const OutputOptions &outputOptions) {}

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

      // switch (input.GetType()) {
      // case file::Type::Stone: {
      //   hc.currentRequest =
      //       compilation.GetDriver().MakeRequest<CompileJobRequest>(
      //           hc.currentRequest,
      //           compilation.GetDriver().GetOutputFileType());
      //   hc.AddModuleInput(hc.currentRequest);
      //   if (outputOptions.CanLink()) {
      //     hc.AddLinkInput(hc.currentRequest);
      //   }
      //   break;
      // }
      // case file::Type::Object:
      //   if (outputOptions.CanLink()) {
      //     hc.AddLinkInput(hc.currentRequest);
      //     break;
      //   }
      // default:
      //   stone::Panic("Alien file -- cannot build job request");
      // }
    }
  }
}

void Driver::BuildCompilationJobs(Compilation &compilation,
                                  const file::Files &inputs,
                                  const OutputOptions &outputOptions) {

  JobCache jc;
  STONE_DEFER { jc.Finish(); };

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
void JobCache::Finish() {

  //
}