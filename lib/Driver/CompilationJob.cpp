#include "stone/Driver/CompilationJob.h"

using namespace stone;

/// Print a nice summary of this job
void CompilationJob::Print(ColorOutputStream &stream) {

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

/// Perform a complete dump of this job.
void CompilationJob::Dump(ColorOutputStream &stream,
                          llvm::StringRef terminator) {

  //
}
// void CompileJob::Run() { cmd::ExecuteAsync(*ToCommand()); }

// const Command *CompileJob::ToCommand() const {
//   // TODO: Build out command

//   return llvm::dyn_cast<Command>(this);
// }

// void DynamicLinkJob::Run() { cmd::ExecuteAsync(*ToCommand()); }

// const Command *DynamicLinkJob::ToCommand() const {
//   // TODO: Build out command

//   return llvm::dyn_cast<Command>(this);
// }
