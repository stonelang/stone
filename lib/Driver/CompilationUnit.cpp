#include "stone/Driver/CompilationUnit.h"
#include "stone/Core/Defer.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

void Driver::BuildCompilationUnits(Compilation &compilation,
                                   const file::Files &inputs,
                                   const OutputOptions &outputOptions) {
  UnitCache uc;
  // STONE_DEFER { jc.Finish(compilation, outputOptions); };
}

/// -print-jobs
void CompilationUnit::Print(ColorOutputStream &stream,
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
void CompilationJob::Dump(ColorOutputStream &stream,
                          llvm::StringRef terminator) const {

  for (auto input : *this) {
  }
}

const char *CompilationUnit::GetNameByKind(CompilationUnitKind kind) {
  switch (kind) {
  case CompilationUnitKind::Input:
    return "input";
  case CompilationUnitKind::Job:
    return "job";
  default:
    stone::Panic("Invalid 'compilation unit'");
  }
}
