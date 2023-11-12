#include "stone/Driver/Compilation.h"
#include "stone/Driver/CompilationJob.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

// CompileJob::CompileJob(Context &ctx, const Tool &tool,
//                        file::Type outputFileType)
//     : Job(JobKind::Compile, ctx, tool, {}, outputFileType) {}

// CompileJob::CompileJob(Context &ctx, const Tool &tool, job::Input input,
//                        file::Type outputFileType)
//     : Job(JobKind::Compile, ctx, tool, input, outputFileType),
//       primaryInput(input) {}

// /// Print a nice summary of this job
// void CompileJob::Print(ColorStream &stream, CrashState *crashState) {

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
//}

/// Perform a complete dump of this job.
// void CompileJob::Dump(ColorStream &stream, llvm::StringRef terminator,
//                       CrashState *crashState) {}

// std::unique_ptr<TaskDetail> CompileJob::ToTaskDetail(ToolChain &tc) const {
//   return tc.ToTaskDetail(*this);
// }

// CompileJob::CompileJob(Phase &action, Context &ctx, Tool &tool)
//     : Job(action, ctx, tool) {}

// int CompileJob::ExecuteAsync() { return Job::ExecuteAsync(); }
// int CompileJob::ExecuteSync() { return Job::ExecuteSync(); }

// void CompileJob::PrintPhase() {

//   /// TODO: PhaseFormatter
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
