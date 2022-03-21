#include "stone/Core/Defer.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Activity.h"
#include "stone/Driver/Driver.h"

using namespace stone;

void Driver::BuildActivities(Compilation &compilation,
                                   const file::Files &inputs,
                                   const OutputOptions &outputOptions) {
  //ActivityCache uc;
  // STONE_DEFER { jc.Finish(compilation, outputOptions); };
}

/// -print-jobs
void Activity::Print(ColorOutputStream &stream,
                     llvm::StringRef terminator) const {

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
void JobActivity::Dump(ColorOutputStream &stream,
                       llvm::StringRef terminator) const {

  for (auto input : *this) {
  }
}

const char *Activity::GetNameByKind(ActivityKind kind) {
  switch (kind) {
  case ActivityKind::Input:
    return "input";
  case ActivityKind::Compile:
    return "compile";
  case ActivityKind::StaticLink:
    return "static-link";
  case ActivityKind::DynamicLink:
    return "dynamic-link";
  case ActivityKind::ExecLink:
    return "exec-link";
  default:
    stone::Panic("Invalid 'compilation unit'");
  }
}
