#include "stone/Driver/Compilation.h"
#include "stone/Driver/CompilationJob.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

// Some jobs only consume inputs -- For example, LinkJob
// LinkJob::LinkJob(Action &action, Context &ctx, Tool &tool, bool requiresLTO,
//                  LinkMode linkMode)
//     : Job(action, ctx, tool), requiresLTO(requiresLTO), linkMode(linkMode)
//     {}

// void LinkJob::PrintAction() {

//   OS() << std::to_string(GetQueueID()) << ":";
//   OS().UseGreen();
//   OS() << GetName();
//   OS().Reset();
//   OS() << "(";

//   int depSize = deps.size();
//   for (auto &dep : deps) {
//     --depSize;
//     OS() << std::to_string(dep.GetQueueID());
//     if (depSize != 0) {
//       OS() << ",";
//     }
//   }
//   OS() << ")"
//        << " -> "
//        << "image" << '\n';
// }

// // Some jobs only consume inputs -- For example, LinkJob
// StaticLinkJob::StaticLinkJob(Action &action, Context &ctx, Tool &tool,
//                              bool requiresLTO)
//     : LinkJob(action, ctx, tool, requiresLTO, LinkMode::EmitStaticLibrary)
//     {}

// int StaticLinkJob::ExecuteAsync() { return Job::ExecuteAsync(); }
// int StaticLinkJob::ExecuteSync() { return Job::ExecuteSync(); }

// // void StaticLinkJob::BuildCmdOutput() {}

// // Some jobs only consume inputs -- For example, LinkJob
// DynamicLinkJob::DynamicLinkJob(Action &action, Context &ctx, Tool &tool,
//                                bool requiresLTO)
//     : LinkJob(action, ctx, tool, requiresLTO, LinkMode::EmitDynamicLibrary)
//     {}
