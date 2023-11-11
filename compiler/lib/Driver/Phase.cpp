#include "stone/Driver/Phase.h"
#include "stone/Basic/Color.h"
#include "stone/Public.h"

using namespace stone;

Phase::Phase(PhaseKind kind, const Tool &tool, PhaseInputList inputs,
             file::Type outputFileType)
    : kind(kind), tool(tool), inputs(inputs), outputFileType(outputFileType) {}

Phase::~Phase() {}

CompilePhase::CompilePhase(const Tool &tool, file::Type outputFileType)
    : Phase(PhaseKind::Compile, tool, {}, outputFileType) {}

CompilePhase::CompilePhase(const Tool &tool, PhaseInput input,
                           file::Type outputFileType)
    : Phase(PhaseKind::Compile, tool, input, outputFileType),
      primaryInput(input) {}

DynamicLinkPhase::DynamicLinkPhase(const Tool &tool, PhaseInputList inputs,
                                   bool withLTO)
    : Phase(PhaseKind::DynamicLink, tool, inputs, file::Type::Image),
      withLTO(withLTO) {}

StaticLinkPhase::StaticLinkPhase(const Tool &tool, PhaseInputList inputs)
    : Phase(PhaseKind::StaticLink, tool, inputs, file::Type::Image) {}

ExecutableLinkPhase::ExecutableLinkPhase(const Tool &tool,
                                         PhaseInputList inputs)
    : Phase(PhaseKind::ExecutableLink, tool, inputs, file::Type::Image) {}

const char *Phase::GetNameByKind(PhaseKind kind) const {
  switch (kind) {
  case PhaseKind::Compile:
    return "compile";
  case PhaseKind::Backend:
    return "backend";
  case PhaseKind::Assemble:
    return "assemble";
  case PhaseKind::DynamicLink:
    return "dynamic-link";
  case PhaseKind::StaticLink:
    return "static-link";
  case PhaseKind::ExecutableLink:
    return "executable-link";
  default:
    stone::Panic("Invalid PhaseKind");
  }
}

static void PrintPhase(ColorStream &stream, llvm::StringRef terminator,
                       const PhaseInput) {
  //   /// TODO: PhaseFormatter
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

void Phase::Print(ColorStream &stream, llvm::StringRef terminator) const {
  for (auto input : *this) {
    PrintPhase(stream, terminator, input);
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
//   compilation.GetDriver().MakePhase<CompileJobRequest>(
//       compilation.GetDriver().GetOutputFileType());
//   for (auto &input : inputs) {
//     if (compilation.GetDriver().GetBuildSystem().IsDirty(input)) {
//       assert(input.GetType() == compilation.GetDriver().GetInputFileType() &&
//              "Incompatible input file types");

//       assert(file::IsPartOfCompilation(input.GetType()));
//       compileRequest->AddInput(
//           compilation.GetDriver().MakePhase<InputRequest>(input));

//       hc.GetPhaseCache().CacheForModule(compileRequest);
//       if (outputOptions.CanLink()) {
//         hc.GetPhaseCache().CacheForLink(hc.GetPhaseCache().currentRequest);
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

//       hc.GetPhaseCache().currentRequest =
//       driver.MakePhase<InputRequest>(input); switch (input.GetType()) {
//       case file::Type::Stone: {
//         hc.GetPhaseCache().currentRequest =
//         driver.MakePhase<CompileJobRequest>(
//             hc.GetPhaseCache().currentRequest, driver.GetOutputFileType());
//         hc.GetPhaseCache().CacheForModule(hc.GetPhaseCache().currentRequest);
//         if (outputOptions.CanLink()) {
//           hc.GetPhaseCache().CacheForLink(hc.GetPhaseCache().currentRequest);
//         }
//         break;
//       }
//       case file::Type::Object:
//         if (outputOptions.CanLink()) {
//           hc.GetPhaseCache().CacheForLink(hc.GetPhaseCache().currentRequest);
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
//   if (outputOptions.CanLink() && hc.GetPhaseCache().ForLink()) {
//     Request *linkRequest = nullptr;
//     switch (GetLinkMode()) {
//     case LinkMode::EmitExecutable: {
//       linkRequest = MakePhase<LinkJobRequest>(hc.GetPhaseCache().forLink,
//                                                 GetLinkMode(), false);
//       break;
//     }
//     case LinkMode::EmitDynamicLibrary: {
//       linkRequest = MakePhase<LinkJobRequest>(
//           hc.GetPhaseCache().forLink, GetLinkMode(),
//           outputOptions.WithLTO());
//       break;
//     }
//     case LinkMode::EmitStaticLibrary: {
//       linkRequest = MakePhase<LinkJobRequest>(hc.GetPhaseCache().forLink,
//                                                 GetLinkMode(), false);
//       break;
//     }
//     default:
//       stone::Panic("Invalid linking mode");
//     }
//     assert(linkRequest);
//     hc.GetPhaseCache().CacheForTop(linkRequest);
//   }
// }

// void Driver::PrintJobRequests(HotCache &hc) {
//   // Let just handle top level
//   if (hc.GetPhaseCache().ForTop()) {
//     for (auto &request : hc.GetPhaseCache().forTop) {
//       request->Print(GetLangContext().Out());
//     }
//   }
// }
