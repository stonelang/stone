#include "stone/Driver/Intent.h"
#include "stone/Basic/Color.h"
#include "stone/Context.h"

using namespace stone;

Intent::Intent(IntentKind kind, const Tool &tool, intent::InputList inputs,
               file::Type outputFileType)
    : kind(kind), tool(tool), inputs(inputs), outputFileType(outputFileType) {}

Intent::~Intent() {}

CompileIntent::CompileIntent(const Tool &tool, file::Type outputFileType)
    : Intent(IntentKind::Compile, tool, {}, outputFileType) {}

CompileIntent::CompileIntent(const Tool &tool, intent::Input input,
                             file::Type outputFileType)
    : Intent(IntentKind::Compile, tool, input, outputFileType),
      primaryInput(input) {}

DynamicLinkIntent::DynamicLinkIntent(const Tool &tool, intent::InputList inputs,
                                     bool withLTO)
    : Intent(IntentKind::DynamicLink, tool, inputs, file::Type::Image),
      withLTO(withLTO) {}

StaticLinkIntent::StaticLinkIntent(const Tool &tool, intent::InputList inputs)
    : Intent(IntentKind::StaticLink, tool, inputs, file::Type::Image) {}

ExecutableLinkIntent::ExecutableLinkIntent(const Tool &tool,
                                           intent::InputList inputs)
    : Intent(IntentKind::ExecutableLink, tool, inputs, file::Type::Image) {}

const char *Intent::GetNameByKind(IntentKind kind) const {
  switch (kind) {
  case IntentKind::Compile:
    return "compile";
  case IntentKind::Backend:
    return "backend";
  case IntentKind::Assemble:
    return "assemble";
  case IntentKind::DynamicLink:
    return "dynamic-link";
  case IntentKind::StaticLink:
    return "static-link";
  case IntentKind::ExecutableLink:
    return "executable-link";
  default:
    stone::Panic("Invalid IntentKind");
  }
}

static void PrintIntent(ColorfulStream &stream, llvm::StringRef terminator,
                        const intent::Input) {
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

void Intent::Print(ColorfulStream &stream, llvm::StringRef terminator) const {
  for (auto input : *this) {
    PrintIntent(stream, terminator, input);
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
//   compilation.GetDriver().MakeIntent<CompileJobRequest>(
//       compilation.GetDriver().GetOutputFileType());
//   for (auto &input : inputs) {
//     if (compilation.GetDriver().GetBuildSystem().IsDirty(input)) {
//       assert(input.GetType() == compilation.GetDriver().GetInputFileType() &&
//              "Incompatible input file types");

//       assert(file::IsPartOfCompilation(input.GetType()));
//       compileRequest->AddInput(
//           compilation.GetDriver().MakeIntent<InputRequest>(input));

//       hc.GetIntentCache().CacheForModule(compileRequest);
//       if (outputOptions.CanLink()) {
//         hc.GetIntentCache().CacheForLink(hc.GetIntentCache().currentRequest);
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

//       hc.GetIntentCache().currentRequest =
//       driver.MakeIntent<InputRequest>(input); switch (input.GetType()) {
//       case file::Type::Stone: {
//         hc.GetIntentCache().currentRequest =
//         driver.MakeIntent<CompileJobRequest>(
//             hc.GetIntentCache().currentRequest, driver.GetOutputFileType());
//         hc.GetIntentCache().CacheForModule(hc.GetIntentCache().currentRequest);
//         if (outputOptions.CanLink()) {
//           hc.GetIntentCache().CacheForLink(hc.GetIntentCache().currentRequest);
//         }
//         break;
//       }
//       case file::Type::Object:
//         if (outputOptions.CanLink()) {
//           hc.GetIntentCache().CacheForLink(hc.GetIntentCache().currentRequest);
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
//   if (outputOptions.CanLink() && hc.GetIntentCache().ForLink()) {
//     Request *linkRequest = nullptr;
//     switch (GetLinkMode()) {
//     case LinkMode::EmitExecutable: {
//       linkRequest = MakeIntent<LinkJobRequest>(hc.GetIntentCache().forLink,
//                                                 GetLinkMode(), false);
//       break;
//     }
//     case LinkMode::EmitDynamicLibrary: {
//       linkRequest = MakeIntent<LinkJobRequest>(
//           hc.GetIntentCache().forLink, GetLinkMode(),
//           outputOptions.WithLTO());
//       break;
//     }
//     case LinkMode::EmitStaticLibrary: {
//       linkRequest = MakeIntent<LinkJobRequest>(hc.GetIntentCache().forLink,
//                                                 GetLinkMode(), false);
//       break;
//     }
//     default:
//       stone::Panic("Invalid linking mode");
//     }
//     assert(linkRequest);
//     hc.GetIntentCache().CacheForTop(linkRequest);
//   }
// }

// void Driver::PrintJobRequests(HotCache &hc) {
//   // Let just handle top level
//   if (hc.GetIntentCache().ForTop()) {
//     for (auto &request : hc.GetIntentCache().forTop) {
//       request->Print(GetContext().Out());
//     }
//   }
// }
