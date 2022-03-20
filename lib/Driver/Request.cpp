#include "stone/Core/File.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

const char *Request::GetNameByKind(RequestKind kind) {
  switch (kind) {
  case RequestKind::Compile:
    return "compile";
  case RequestKind::Link:
    return "link";
  default:
    stone::Panic("Invalid JobRequest");
  }
}

void Request::Print(ColorOutputStream &stream,
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

void JobRequest::Print(ColorOutputStream &stream,
                       llvm::StringRef terminator) const {
  for (auto jr : *this) {
    jr->Print(stream, terminator);
  }
}

static void BuildSingleCompilingModelKind(Compilation &comp, HotCache &chi,
                                          const file::Files &inputs,
                                          const OutputOptions &outputOptions) {}

static void BuildBatchCompilingModelKind(Compilation &comp, HotCache &chi,
                                         const file::Files &inputs,
                                         const OutputOptions &outputOptions) {}

static void BuildLinkJobRequest(Compilation &comp, HotCache &hc,
                                const OutputOptions &outputOptions) {

  // Now, do we need any top-level JobRequests
  if (outputOptions.CanLink() && hc.HasLinkInputs()) {

    Request *linkRequest = nullptr;
    switch (comp.GetDriver().GetLinkMode()) {
    case LinkMode::EmitExecutable: {
      linkRequest = comp.GetDriver().MakeRequest<LinkJobRequest>(
          hc.linkInputs, comp.GetDriver().GetLinkMode(), false);
      break;
    }
    case LinkMode::EmitDynamicLibrary: {
      linkRequest = comp.GetDriver().MakeRequest<LinkJobRequest>(
          hc.linkInputs, comp.GetDriver().GetLinkMode(),
          outputOptions.RequiresLTO());
      break;
    }
    case LinkMode::EmitStaticLibrary: {
      linkRequest = comp.GetDriver().MakeRequest<LinkJobRequest>(
          hc.linkInputs, comp.GetDriver().GetLinkMode(), false);
      break;
    }
    default:
      stone::Panic("Invalid linking mode");
    }
    assert(linkRequest);
    hc.AddTopLevelRequest(linkRequest);
  }
}

static void BuildCompileJobRequest(Compilation &comp, HotCache &hc,
                                   const Request *request,
                                   const OutputOptions &outputOptions) {
  /// Since you are here, you could just get the tool -- this will
  /// be done in the ConstructInvocatin calls.

  // auto tool = comp.GetToolChain().FindTool(ToolKind::SC);
  // assert(tool && "Could not find stone-compile tool!");
  hc.currentRequest = comp.GetDriver().MakeRequest<CompileJobRequest>(
      request, comp.GetDriver().GetOutputFileType());

  // TODO: Think about this
  hc.AddModuleInput(hc.currentRequest);

  if (outputOptions.CanLink()) {
    hc.AddLinkInput(hc.currentRequest);
  }
}

static void
BuildMultipleCompilingModelKind(Compilation &comp, HotCache &hc,
                                const file::Files &inputs,
                                const OutputOptions &outputOptions) {
  for (auto &input : inputs) {
    // TODO: Way out there, but there is potential for git here?
    if (comp.GetDriver().GetBuildSystem().IsDirty(input)) {

      assert(input.GetType() == comp.GetDriver().GetInputFileType() &&
             "Incompatible input file types");
      assert(file::IsPartOfCompilation(input.GetType()));

      hc.currentRequest = comp.GetDriver().MakeRequest<InputRequest>(input);

      switch (input.GetType()) {
      case file::Type::Stone:
        BuildCompileJobRequest(comp, hc, hc.currentRequest, outputOptions);
        break;
      case file::Type::Object:
        if (outputOptions.CanLink()) {
          hc.AddLinkInput(hc.currentRequest);
          break;
        }
      default:
        stone::Panic("Alien file -- cannot build job request");
      }
    }
  }
  BuildLinkJobRequest(comp, hc, outputOptions);
}

void Driver::BuildJobRequests(Compilation &comp, HotCache &hc,
                              const file::Files &inputs,
                              const OutputOptions &outputOptions) {

  // We assert here because this should have been checked above.
  assert(inputs.empty());

  switch (driverOpts.outputOptions.compilingModelKind) {
  case CompilingModelKind::Multiple:
    BuildMultipleCompilingModelKind(comp, hc, inputs, outputOptions);
    break;
  case CompilingModelKind::Single:
    BuildSingleCompilingModelKind(comp, hc, inputs, outputOptions);
    break;
  case CompilingModelKind::Batch:
    BuildBatchCompilingModelKind(comp, hc, inputs, outputOptions);
    break;
  default:
    stone::Panic("Unsupported Compiling mode");
  }
}

void Driver::PrintJobRequests(const HotCache &hc) {

  // Let just handle top level
  if (hc.HasTopLevelRequest()) {
    for (auto &request : hc.topLevelRequests) {
      request->Print(GetContext().Out());
    }
  }
}
