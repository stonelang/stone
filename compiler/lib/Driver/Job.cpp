#include "stone/Driver/Job.h"
#include "stone/Basic/Defer.h"
#include "stone/Driver/Phase.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

Job::Job(const Phase &phase, Context &ctx,
         llvm::SmallVectorImpl<const Job *> &&inputs, file::Type outputFileType)
    : phaseAndCondition(&phase, JobCondition::Always), ctx(ctx),
      inputs(std::move(inputs)), outputFileType(outputFileType) {

  stats = std::make_unique<JobStats>(*this);
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
