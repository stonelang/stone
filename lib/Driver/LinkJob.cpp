#include "stone/Driver/Compilation.h"
#include "stone/Driver/CompilationJob.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

// Some jobs only consume inputs -- For example, LinkJob
// LinkJob::LinkJob(Intent &intent, Context &ctx, Tool &tool, bool requiresLTO,
//                  LinkMode linkMode)
//     : Job(intent, ctx, tool), requiresLTO(requiresLTO), linkMode(linkMode)
//     {}

// void LinkJob::PrintIntent() {

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

// std::unique_ptr<TaskDetail> StaticLinkJob::ToTaskDetail() const {

//   return nullptr;
// }

// std::unique_ptr<TaskDetail> DynamicLinkJob::ToTaskDetail() const {

//   return nullptr;
// }

// std::unique_ptr<TaskDetail> ExecutableLinkJob::ToTaskDetail() const {

//   return nullptr;
// }

// // Some jobs only consume inputs -- For example, LinkJob
// StaticLinkJob::StaticLinkJob(Intent &intent, Context &ctx, Tool &tool,
//                              bool requiresLTO)
//     : LinkJob(intent, ctx, tool, requiresLTO, LinkMode::EmitStaticLibrary)
//     {}

// int StaticLinkJob::ExecuteAsync() { return Job::ExecuteAsync(); }
// int StaticLinkJob::ExecuteSync() { return Job::ExecuteSync(); }

// // void StaticLinkJob::BuildCmdOutput() {}

// // Some jobs only consume inputs -- For example, LinkJob
// DynamicLinkJob::DynamicLinkJob(Intent &intent, Context &ctx, Tool &tool,
//                                bool requiresLTO)
//     : LinkJob(intent, ctx, tool, requiresLTO, LinkMode::EmitDynamicLibrary)
//     {}
