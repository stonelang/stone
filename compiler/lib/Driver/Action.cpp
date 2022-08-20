#include "stone/Driver/Action.h"
#include "stone/Basic/Color.h"
#include "stone/Context.h"

using namespace stone;

Action::Action(ActionKind kind, const Tool &tool, action::InputList inputs,
               file::Type outputFileType)
    : kind(kind), tool(tool), inputs(inputs), outputFileType(outputFileType) {}

Action::~Action() {}

CompileAction::CompileAction(const Tool &tool, file::Type outputFileType)
    : Action(ActionKind::Compile, tool, {}, outputFileType) {}

CompileAction::CompileAction(const Tool &tool, action::Input input,
                             file::Type outputFileType)
    : Action(ActionKind::Compile, tool, input, outputFileType),
      primaryInput(input) {}

DynamicLinkAction::DynamicLinkAction(const Tool &tool, action::InputList inputs,
                                     bool withLTO)
    : Action(ActionKind::DynamicLink, tool, inputs, file::Type::Image),
      withLTO(withLTO) {}

StaticLinkAction::StaticLinkAction(const Tool &tool, action::InputList inputs)
    : Action(ActionKind::StaticLink, tool, inputs, file::Type::Image) {}

ExecutableLinkAction::ExecutableLinkAction(const Tool &tool,
                                           action::InputList inputs)
    : Action(ActionKind::ExecutableLink, tool, inputs, file::Type::Image) {}

const char *Action::GetNameByKind(ActionKind kind) const {
  switch (kind) {
  case ActionKind::Compile:
    return "compile";
  case ActionKind::Backend:
    return "backend";
  case ActionKind::Assemble:
    return "assemble";
  case ActionKind::DynamicLink:
    return "dynamic-link";
  case ActionKind::StaticLink:
    return "static-link";
  case ActionKind::ExecutableLink:
    return "executable-link";
  default:
    stone::Panic("Invalid ActionKind");
  }
}

static void PrintAction(ColorfulStream &stream, llvm::StringRef terminator,
                        const action::Input) {
  //   /// TODO: ActionFormatter
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

void Action::Print(ColorfulStream &stream, llvm::StringRef terminator) const {
  for (auto input : *this) {
    PrintAction(stream, terminator, input);
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
//   compilation.GetDriver().MakeAction<CompileJobRequest>(
//       compilation.GetDriver().GetOutputFileType());
//   for (auto &input : inputs) {
//     if (compilation.GetDriver().GetBuildSystem().IsDirty(input)) {
//       assert(input.GetType() == compilation.GetDriver().GetInputFileType() &&
//              "Incompatible input file types");

//       assert(file::IsPartOfCompilation(input.GetType()));
//       compileRequest->AddInput(
//           compilation.GetDriver().MakeAction<InputRequest>(input));

//       hc.GetActionCache().CacheForModule(compileRequest);
//       if (outputOptions.CanLink()) {
//         hc.GetActionCache().CacheForLink(hc.GetActionCache().currentRequest);
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

//       hc.GetActionCache().currentRequest =
//       driver.MakeAction<InputRequest>(input); switch (input.GetType()) {
//       case file::Type::Stone: {
//         hc.GetActionCache().currentRequest =
//         driver.MakeAction<CompileJobRequest>(
//             hc.GetActionCache().currentRequest, driver.GetOutputFileType());
//         hc.GetActionCache().CacheForModule(hc.GetActionCache().currentRequest);
//         if (outputOptions.CanLink()) {
//           hc.GetActionCache().CacheForLink(hc.GetActionCache().currentRequest);
//         }
//         break;
//       }
//       case file::Type::Object:
//         if (outputOptions.CanLink()) {
//           hc.GetActionCache().CacheForLink(hc.GetActionCache().currentRequest);
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
//   if (outputOptions.CanLink() && hc.GetActionCache().ForLink()) {
//     Request *linkRequest = nullptr;
//     switch (GetLinkMode()) {
//     case LinkMode::EmitExecutable: {
//       linkRequest = MakeAction<LinkJobRequest>(hc.GetActionCache().forLink,
//                                                 GetLinkMode(), false);
//       break;
//     }
//     case LinkMode::EmitDynamicLibrary: {
//       linkRequest = MakeAction<LinkJobRequest>(
//           hc.GetActionCache().forLink, GetLinkMode(),
//           outputOptions.WithLTO());
//       break;
//     }
//     case LinkMode::EmitStaticLibrary: {
//       linkRequest = MakeAction<LinkJobRequest>(hc.GetActionCache().forLink,
//                                                 GetLinkMode(), false);
//       break;
//     }
//     default:
//       stone::Panic("Invalid linking mode");
//     }
//     assert(linkRequest);
//     hc.GetActionCache().CacheForTop(linkRequest);
//   }
// }

// void Driver::PrintJobRequests(HotCache &hc) {
//   // Let just handle top level
//   if (hc.GetActionCache().ForTop()) {
//     for (auto &request : hc.GetActionCache().forTop) {
//       request->Print(GetContext().Out());
//     }
//   }
// }
