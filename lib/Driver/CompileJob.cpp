#include "stone/Driver/Compilation.h"
#include "stone/Driver/CompilationJob.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

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
