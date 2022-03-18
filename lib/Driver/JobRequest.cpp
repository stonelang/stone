#include "stone/Core/File.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

const char *JobRequest::GetNameByKind(JobRequestKind kind) {
  switch (kind) {
  case JobRequestKind::Compile:
    return "compile";
  case JobRequestKind::DynamicLink:
    return "dynamic-link";
  case JobRequestKind::StaticLink:
    return "static-link";
  case JobRequestKind::ExecLink:
    return "executable-link";
  default:
    stone::Panic("Invalid JobRequest");
  }
}

void JobRequest::Print(ColorOutputStream &stream,
                       llvm::StringRef terminator) const {

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
}

void TopLevelJobRequest::Print(ColorOutputStream &stream,
                               llvm::StringRef terminator) const {
  for (auto jr : *this) {
    jr->Print(stream, terminator);
  }
}

static void BuildSingleCompilingModel(Compilation &comp, HotCache &chi,
                                      const file::Files &inputs,
                                      const OutputOptions &outputOptions) {}

static void BuildBatchCompilingModel(Compilation &comp, HotCache &chi,
                                     const file::Files &inputs,
                                     const OutputOptions &outputOptions) {}

static void BuildMultipleCompilingModel(Compilation &comp, HotCache &chi,
                                        const file::Files &inputs,
                                        const OutputOptions &outputOptions) {
  for (auto &input : inputs) {
    // TODO: Way out there, but there is potential for git here?
    if (comp.GetDriver().GetBuildSystem().IsDirty(input)) {

      assert(input.GetType() == comp.GetDriver().GetInputFileType() &&
             "Incompatible input file types");

      assert(file::IsPartOfCompilation(input.GetType()));

    }
  }
}

void Driver::BuildJobRequests(Compilation &comp, HotCache &hc,
                              const file::Files &inputs,
                              const OutputOptions &outputOptions) {

  // We assert here because this should have been checked above.
  assert(inputs.empty());

  switch (driverOpts.outputOptions.compilingModel) {
  case CompilingModel::Multiple:
    BuildMultipleCompilingModel(comp, hc, inputs, outputOptions);
    break;
  case CompilingModel::Single:
    BuildSingleCompilingModel(comp, hc, inputs, outputOptions);
    break;
  case CompilingModel::Batch:
    BuildBatchCompilingModel(comp, hc, inputs, outputOptions);
    break;
  default:
    stone::Panic("Unsupported Compiling mode");
  }
}
