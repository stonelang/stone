#include "stone/Driver/Compilation.h"
#include "stone/Driver/CompilationJob.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

CompileJob::CompileJob(Context &ctx, const Tool &tool,
                       file::Type outputFileType)
    : Job(JobKind::Compile, ctx, tool, {}, outputFileType) {}

CompileJob::CompileJob(Context &ctx, const Tool &tool, job::Input input,
                       file::Type outputFileType)
    : Job(JobKind::Compile, ctx, tool, input, outputFileType) {}

/// Print a nice summary of this job
void CompileJob::Print(ColorOutputStream &stream,
                       CrashState *crashState) {}

/// Perform a complete dump of this job.
void CompileJob::Dump(ColorOutputStream &stream,
                      llvm::StringRef terminator,
                      CrashState *crashState ) {}

// CompileJob::CompileJob(Intent &intent, Context &ctx, Tool &tool)
//     : Job(intent, ctx, tool) {}

// int CompileJob::ExecuteAsync() { return Job::ExecuteAsync(); }
// int CompileJob::ExecuteSync() { return Job::ExecuteSync(); }

// void CompileJob::PrintIntent() {

//   /// TODO: IntentFormatter
//   OS() << std::to_string(GetQueueID()) << ":";
//   OS().UseGreen();
//   OS() << GetName();
//   OS().Reset();
//   OS() << "(";
//   int inputSize = inputs.size();
//   for (auto &input : inputs) {
//     --inputSize;
//     OS() << input.GetName();
//     if (inputSize != 0) {
//       OS() << ",";
//     }
//   }
//   OS() << ")"
//        << " -> "
//        << "object" << '\n';
//   OS() << '\n';
// }
