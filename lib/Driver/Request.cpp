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

static void BuildBatchCompilingModel(Compilation &compilation, HotCache &chi,
                                     const file::Files &inputs,
                                     const OutputOptions &outputOptions) {}

static void BuildSingleCompilingModel(Compilation &compilation, HotCache &hc,
                                      const file::Files &inputs,
                                      const OutputOptions &outputOptions) {
  // Create a single CompileJobRequest to handl all InputRequest(s)
  auto *compileRequest = compilation.GetDriver().MakeRequest<CompileJobRequest>(
      compilation.GetDriver().GetOutputFileType());
  for (auto &input : inputs) {
    if (compilation.GetDriver().GetBuildSystem().IsDirty(input)) {
      assert(input.GetType() == compilation.GetDriver().GetInputFileType() &&
             "Incompatible input file types");

      assert(file::IsPartOfCompilation(input.GetType()));
      compileRequest->AddInput(
          compilation.GetDriver().MakeRequest<InputRequest>(input));

      hc.GetReqCache().CacheForModule(compileRequest);
      if (outputOptions.CanLink()) {
        hc.GetReqCache().CacheForLink(hc.GetReqCache().currentRequest);
      }
    }
  }
}

static void BuildMultipleCompilingModel(Compilation &compilation, HotCache &hc,
                                        const file::Files &inputs,
                                        const OutputOptions &outputOptions) {
  auto &tc = compilation.GetToolChain();
  auto &driver = compilation.GetDriver();

  for (auto &input : inputs) {
    // TODO: Way out there, but there is potential for git here?
    if (compilation.GetDriver().GetBuildSystem().IsDirty(input)) {
      assert(input.GetType() == compilation.GetDriver().GetInputFileType() &&
             "Incompatible input file types");
      assert(file::IsPartOfCompilation(input.GetType()));

      hc.GetReqCache().currentRequest = driver.MakeRequest<InputRequest>(input);
      switch (input.GetType()) {
      case file::Type::Stone: {
        hc.GetReqCache().currentRequest = driver.MakeRequest<CompileJobRequest>(
            hc.GetReqCache().currentRequest, driver.GetOutputFileType());
        hc.GetReqCache().CacheForModule(hc.GetReqCache().currentRequest);
        if (outputOptions.CanLink()) {
          hc.GetReqCache().CacheForLink(hc.GetReqCache().currentRequest);
        }
        break;
      }
      case file::Type::Object:
        if (outputOptions.CanLink()) {
          hc.GetReqCache().CacheForLink(hc.GetReqCache().currentRequest);
          break;
        }
      default:
        stone::Panic("Alien file -- cannot build job request");
      }
    }
  }
}

void Driver::BuildJobRequests(Compilation &compilation, HotCache &hc,
                              const file::Files &inputs,
                              const OutputOptions &outputOptions) {
  // We assert here because this should have been checked above.
  assert(inputs.empty());

  switch (GetCompilingModelKind()) {
  case CompilingModelKind::Multiple:
    BuildMultipleCompilingModel(compilation, hc, inputs, outputOptions);
    break;
  case CompilingModelKind::Single:
    BuildSingleCompilingModel(compilation, hc, inputs, outputOptions);
    break;
  case CompilingModelKind::Batch:
    BuildBatchCompilingModel(compilation, hc, inputs, outputOptions);
    break;
  default:
    stone::Panic("Unsupported Compiling mode");
  }

  // Now, do we need any top-level JobRequests
  if (outputOptions.CanLink() && hc.GetReqCache().ForLink()) {
    Request *linkRequest = nullptr;
    switch (GetLinkMode()) {
    case LinkMode::EmitExecutable: {
      linkRequest = MakeRequest<LinkJobRequest>(hc.GetReqCache().forLink,
                                                GetLinkMode(), false);
      break;
    }
    case LinkMode::EmitDynamicLibrary: {
      linkRequest = MakeRequest<LinkJobRequest>(
          hc.GetReqCache().forLink, GetLinkMode(), outputOptions.RequiresLTO());
      break;
    }
    case LinkMode::EmitStaticLibrary: {
      linkRequest = MakeRequest<LinkJobRequest>(hc.GetReqCache().forLink,
                                                GetLinkMode(), false);
      break;
    }
    default:
      stone::Panic("Invalid linking mode");
    }
    assert(linkRequest);
    hc.GetReqCache().CacheForTop(linkRequest);
  }
}

void Driver::PrintJobRequests(HotCache &hc) {
  // Let just handle top level
  if (hc.GetReqCache().ForTop()) {
    for (auto &request : hc.GetReqCache().forTop) {
      request->Print(GetContext().Out());
    }
  }
}
