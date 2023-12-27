#include "stone/Driver/JobAction.h"
#include "stone/Basic/Color.h"
#include "stone/Public.h"

using namespace stone;

JobAction::JobAction(JobActionKind kind, const Tool &tool,
                     JobActionInputList inputs, file::Type outputFileType)
    : kind(kind), tool(tool), inputs(inputs), outputFileType(outputFileType) {}

JobAction::~JobAction() {}

CompileJobAction::CompileJobAction(const Tool &tool, file::Type outputFileType)
    : JobAction(JobActionKind::Compile, tool, {}, outputFileType) {}

CompileJobAction::CompileJobAction(const Tool &tool, JobActionInput input,
                                   file::Type outputFileType)
    : JobAction(JobActionKind::Compile, tool, input, outputFileType),
      primaryInput(input) {}

DynamicLinkJobAction::DynamicLinkJobAction(const Tool &tool,
                                           JobActionInputList inputs,
                                           bool withLTO)
    : JobAction(JobActionKind::DynamicLink, tool, inputs, file::Type::Image),
      withLTO(withLTO) {}

StaticLinkJobAction::StaticLinkJobAction(const Tool &tool,
                                         JobActionInputList inputs)
    : JobAction(JobActionKind::StaticLink, tool, inputs, file::Type::Image) {}

ExecutableLinkJobAction::ExecutableLinkJobAction(const Tool &tool,
                                                 JobActionInputList inputs)
    : JobAction(JobActionKind::ExecutableLink, tool, inputs,
                file::Type::Image) {}

const char *JobAction::GetNameByKind(JobActionKind kind) const {
  switch (kind) {
  case JobActionKind::Compile:
    return "compile";
  case JobActionKind::Backend:
    return "backend";
  case JobActionKind::Assemble:
    return "assemble";
  case JobActionKind::DynamicLink:
    return "dynamic-link";
  case JobActionKind::StaticLink:
    return "static-link";
  case JobActionKind::ExecutableLink:
    return "executable-link";
  default:
    stone::Panic("Invalid JobActionKind");
  }
}

static void PrintJobAction(ColorStream &stream, llvm::StringRef terminator,
                           const JobActionInput) {
  //   /// TODO: JobActionFormatter
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

void JobAction::Print(ColorStream &stream, llvm::StringRef terminator) const {
  for (auto input : *this) {
    PrintJobAction(stream, terminator, input);
  }
}

// static void BuildBatchCompilingModel(Compilation &compilation, HotCache &chi,
//                                      const file::Files &inputs,
//                                      const OutputOptions &outputOptions) {}

// static void BuildSingleCompilingModel(Compilation &compilation, HotCache &hc,
//                                       const file::Files &inputs,
//                                       const OutputOptions &outputOptions) {
//   // Create a single CompileJobRequest to handl all InputRequest(s)
//   auto *compileRequest =
//   compilation.GetDriver().MakeJobAction<CompileJobRequest>(
//       compilation.GetDriver().GetOutputFileType());
//   for (auto &input : inputs) {
//     if (compilation.GetDriver().GetBuildSystem().IsDirty(input)) {
//       assert(input.GetType() == compilation.GetDriver().GetInputFileType() &&
//              "Incompatible input file types");

//       assert(file::IsPartOfCompilation(input.GetType()));
//       compileRequest->AddInput(
//           compilation.GetDriver().MakeJobAction<InputRequest>(input));

//       hc.GetJobActionCache().CacheForModule(compileRequest);
//       if (outputOptions.CanLink()) {
//         hc.GetJobActionCache().CacheForLink(hc.GetJobActionCache().currentRequest);
//       }
//     }
//   }
// }

// static void BuildMultipleCompilingModel(Compilation &compilation, HotCache
// &hc,
//                                         const file::Files &inputs,
//                                         const OutputOptions &outputOptions) {
//   auto &tc = compilation.GetToolChain();
//   auto &driver = compilation.GetDriver();

//   for (auto &input : inputs) {
//     // TODO: Way out there, but there is potential for git here?
//     if (compilation.GetDriver().GetBuildSystem().IsDirty(input)) {
//       assert(input.GetType() == compilation.GetDriver().GetInputFileType() &&
//              "Incompatible input file types");
//       assert(file::IsPartOfCompilation(input.GetType()));

//       hc.GetJobActionCache().currentRequest =
//       driver.MakeJobAction<InputRequest>(input); switch (input.GetType()) {
//       case file::Type::Stone: {
//         hc.GetJobActionCache().currentRequest =
//         driver.MakeJobAction<CompileJobRequest>(
//             hc.GetJobActionCache().currentRequest,
//             driver.GetOutputFileType());
//         hc.GetJobActionCache().CacheForModule(hc.GetJobActionCache().currentRequest);
//         if (outputOptions.CanLink()) {
//           hc.GetJobActionCache().CacheForLink(hc.GetJobActionCache().currentRequest);
//         }
//         break;
//       }
//       case file::Type::Object:
//         if (outputOptions.CanLink()) {
//           hc.GetJobActionCache().CacheForLink(hc.GetJobActionCache().currentRequest);
//           break;
//         }
//       default:
//         stone::Panic("Alien file -- cannot build job request");
//       }
//     }
//   }
// }

// void Driver::BuildJobRequests(Compilation &compilation, HotCache &hc,
//                               const file::Files &inputs,
//                               const OutputOptions &outputOptions) {
//   // We assert here because this should have been checked above.
//   assert(inputs.empty());

//   switch (GetCompilationMode()) {
//   case CompilationMode::Quadratic:
//     BuildMultipleCompilingModel(compilation, hc, inputs, outputOptions);
//     break;
//   case CompilationMode::Single:
//     BuildSingleCompilingModel(compilation, hc, inputs, outputOptions);
//     break;
//   case CompilationMode::CPU:
//     BuildBatchCompilingModel(compilation, hc, inputs, outputOptions);
//     break;
//   default:
//     stone::Panic("Unsupported Compiling mode");
//   }

//   // Now, do we need any top-level JobRequests
//   if (outputOptions.CanLink() && hc.GetJobActionCache().ForLink()) {
//     Request *linkRequest = nullptr;
//     switch (GetLinkMode()) {
//     case LinkMode::EmitExecutable: {
//       linkRequest =
//       MakeJobAction<LinkJobRequest>(hc.GetJobActionCache().forLink,
//                                                 GetLinkMode(), false);
//       break;
//     }
//     case LinkMode::EmitDynamicLibrary: {
//       linkRequest = MakeJobAction<LinkJobRequest>(
//           hc.GetJobActionCache().forLink, GetLinkMode(),
//           outputOptions.WithLTO());
//       break;
//     }
//     case LinkMode::EmitStaticLibrary: {
//       linkRequest =
//       MakeJobAction<LinkJobRequest>(hc.GetJobActionCache().forLink,
//                                                 GetLinkMode(), false);
//       break;
//     }
//     default:
//       stone::Panic("Invalid linking mode");
//     }
//     assert(linkRequest);
//     hc.GetJobActionCache().CacheForTop(linkRequest);
//   }
// }

// void Driver::PrintJobRequests(HotCache &hc) {
//   // Let just handle top level
//   if (hc.GetJobActionCache().ForTop()) {
//     for (auto &request : hc.GetJobActionCache().forTop) {
//       request->Print(GetLangContext().Out());
//     }
//   }
// }
