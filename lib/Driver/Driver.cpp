#include "stone/Driver/Driver.h"
#include "stone/Core/CoreDiagnostic.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Darwin.h"
#include "stone/Driver/Job.h"

#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/LockFileManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

using stone::Driver;
using stone::Job;
using stone::ModeKind;

using namespace stone;

Driver::Driver(llvm::StringRef name, llvm::StringRef path)
    : name(name), path(path) {

  buildSystem = std::make_unique<BuildSystem>(*this);
  SetExcludedFlagsBitmask(opts::NoLangOption);
}

Driver::~Driver() {}

llvm::opt::InputArgList &Driver::ParseArgs(llvm::ArrayRef<const char *> args) {
  auto &ial = Session::ParseArgs(args);
  return ial;
}

void Driver::Initialize() {}

void Driver::ComputeOutputOptions(const ToolChain &toolChain,
                                  const llvm::opt::InputArgList &ial,
                                  const file::Files &inputs,
                                  OutputOptions &outputOptions) {}

std::unique_ptr<Compilation>
Driver::BuildCompilation(ToolChain &toolChain, llvm::opt::InputArgList &ial) {

  llvm::PrettyStackTraceString crashInfo("Building compilation");

  if (driverOpts.cleanBuild) {
    GetBuildSystem().Clean();
  }
  GetBuildSystem().StartBuild();

  auto inputs = BuildInputFiles(ial);
  if (HasError()) {
    return nullptr;
  }

  auto workDir = ComputeWorkDir(ial);
  auto dal = TranslateInputArgList(ial, workDir);

  // /// Think about
  bool isBatchModel = false;
  driverOpts.outputOptions.compilingModelKind =
      ComputeCompilingModelKind(*dal, isBatchModel);

  // ComputeOutputOptions(toolChain, *dal, inputs, driverOpts, batchMode);

  // TODO: Check input size
  // Now, build the job system since we have a toolchain
  auto compilation =
      std::make_unique<Compilation>(*this, toolChain, std::move(dal));

  if (inputs.empty()) {
    GetContext().Printd(SrcLoc(), diag::err_no_input_files);
    return nullptr;
  }

  HotCache hc;
  BuildJobRequests(*compilation, hc, inputs, driverOpts.outputOptions);

  // A quick -print-requests check
  if (driverOpts.printRequests) {
    // PrintJobRequests(hc);
    return nullptr;
  }

  BuildJobs(*compilation, hc, driverOpts.outputOptions);
  return compilation;
}

void Driver::ComputeLinkMode(const llvm::opt::InputArgList &ial) {

  assert(GetMode().IsAlien() && "Alien mode");
  if (GetMode().IsNone()) {
    GetDriverOptions().outputOptions.linkMode = LinkMode::EmitExecutable;
  } else if (GetMode().IsEmitLibrary()) {
    if (ial.hasArg(opts::Static)) {
      GetDriverOptions().outputOptions.linkMode = LinkMode::EmitStaticLibrary;
    } else {
      GetDriverOptions().outputOptions.linkMode = LinkMode::EmitDynamicLibrary;
    }
  }
}

std::unique_ptr<llvm::opt::DerivedArgList>
Driver::TranslateInputArgList(const llvm::opt::InputArgList &ial,
                              llvm::StringRef workDir) {

  auto dal = std::make_unique<llvm::opt::DerivedArgList>(ial);

  // auto addPath = [workingDirectory, DAL](Arg *A) {
  //   assert(A->getNumValues() == 1 && "multiple values not handled");
  //   StringRef path = A->getValue();
  //   if (workingDirectory.empty() || path == "-" ||
  //       llvm::sys::path::is_absolute(path)) {
  //     DAL->append(A);
  //     return;
  //   }
  // }
  return dal;
}

CompilingModelKind
Driver::ComputeCompilingModelKind(const llvm::opt::DerivedArgList &dal,
                                  bool &isBatchModel) const {
  // Just use multiple for now
  return CompilingModelKind::Multiple;
}

std::unique_ptr<ToolChain>
Driver::BuildToolChain(const llvm::opt::InputArgList &argList) {

  if (const llvm::opt::Arg *arg = argList.getLastArg(opts::Target)) {
    ctx.GetSystemOptions().SetTargetTriple(
        llvm::Triple::normalize(arg->getValue()));
  }

  switch (ctx.GetSystemOptions().target.getOS()) {
  case llvm::Triple::Darwin:
  case llvm::Triple::MacOSX: {
    llvm::Optional<llvm::Triple> targetVariant;
    if (const llvm::opt::Arg *A = argList.getLastArg(opts::TargetVariant)) {
      targetVariant = llvm::Triple(llvm::Triple::normalize(A->getValue()));
    }
    return std::make_unique<DarwinToolChain>(
        *this, ctx.GetSystemOptions().target, targetVariant);
  }
  // case llvm::Triple::Linux:
  //   toolChain = std::make_unique<stone::Linux>(*this, target);
  //   break;
  // case llvm::Triple::FreeBSD:
  //   toolChain = std::make_unique<stone::FreeBSD>(*this, target);
  //   break;
  // case llvm::Triple::OpenBSD:
  //   toolChain = std::make_unique<stone::OpenBSD>(*this, target);
  //   break;
  // case llvm::Triple::Win32:
  //   toolChain = std::make_unique<stone::Win>(*this, target);
  //   break;
  default:
    stone::Panic("OS not found!");
  }
}

void Driver::ComputeOptions(const llvm::opt::InputArgList &ial) {

  // Since the mode has already been created
  // switch(GetMode().GetKind().)
  driverOpts.outputFileType = file::Type::Object;

  // TODO:
  // driverOpts.compileModel = ComputeCompilingModelKind(
  //     *tal, driverOpts.inputFiles);

  // auto scPathResult = GetEQValue(opts::LangPathEQ);
  // if (!stPathResult.IsErr()) {
  //   driverOpts.scPath = stPathResult.Get();
  // }

  driverOpts.printRequests = ial.hasArg(opts::PrintDriverRequests);
  driverOpts.printJobs = ial.hasArg(opts::PrintDriverJobs);
  driverOpts.printLifecycle = ial.hasArg(opts::PrintDriverLifecycle);
  driverOpts.systemOpts.printStatistics = ial.hasArg(opts::PrintStats);
}

llvm::StringRef Driver::ComputeOutputFilename() {}

// int Driver::ExecuteJob(const Job &job, const Job *&fallBackJob) const
// {
//   return 0;
// }
// void Driver::ExecuteJobs(
//     llvm::SmallVectorImpl<std::pair<int, const Job *>> &fallBackJob) const
//     {}

void HotCache::Finish(Compilation &compilation,
                      const OutputOptions &outputOpts) {

  //  // Now, do we need any top-level JobRequests
  // if (outputOptions.CanLink() && hc.ForLink()) {

  //   Request *linkRequest = nullptr;
  //   switch (GetLinkMode()) {
  //   case LinkMode::EmitExecutable: {
  //     linkRequest =
  //         MakeRequest<LinkJobRequest>(hc.forLink, GetLinkMode(), false);
  //     break;
  //   }
  //   case LinkMode::EmitDynamicLibrary: {
  //     linkRequest = MakeRequest<LinkJobRequest>(hc.forLink, GetLinkMode(),
  //                                               outputOptions.RequiresLTO());
  //     break;
  //   }
  //   case LinkMode::EmitStaticLibrary: {
  //     linkRequest =
  //         MakeRequest<LinkJobRequest>(hc.forLink, GetLinkMode(), false);
  //     break;
  //   }
  //   default:
  //     stone::Panic("Invalid linking mode");
  //   }
  //   assert(linkRequest);
  //   hc.AddTopLevelRequest(linkRequest);
  // }
}
void Driver::PrintVersion() {}
void Driver::Finish() {}
