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
void Job::Print(ColorfulStream &stream, CrashState *crashState) {

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
void Job::Dump(ColorfulStream &stream, llvm::StringRef terminator,
               CrashState *crashState) {
  //
}
