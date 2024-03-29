#include "stone/Driver/Driver.h"
#include "stone/Diag/CoreDiagnostic.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/DarwinToolChain.h"
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

using stone::ActionKind;
using stone::Driver;
using stone::Job;

using namespace stone;

Driver::Driver(llvm::StringRef programName, llvm::StringRef programPath)
    : Session(programName, programPath) {
  buildSystem = std::make_unique<BuildSystem>(*this);
  excludedFlagsBitmask = opts::NoCompilerOption;
}

Driver::~Driver() {}

void Driver::ComputeOutputOptions(const ToolChain &toolChain,
                                  const llvm::opt::InputArgList &ial,
                                  const file::Files &inputs,
                                  OutputOptions &outputOptions) {}

std::unique_ptr<Compilation>
Driver::BuildCompilation(ToolChain &toolChain, llvm::opt::InputArgList &ial) {
  llvm::PrettyStackTraceString crashInfo("Building compilation");

  if (GetDriverOptions().cleanBuild) {
    GetBuildSystem().Clean();
  }
  GetBuildSystem().StartBuild();

  auto inputs = BuildInputFiles(ial);
  if (HasError()) {
    return nullptr;
  }

  if (inputs.empty()) {
    GetLangContext().GetDiags().PrintD(SrcLoc(), diag::err_no_input_files);
    return nullptr;
  }

  auto workDir = ComputeWorkDir(ial);

  /// TODO: for now...move into ComputeOptions
  ComputeLinkMode(ial);

  auto dal = TranslateInputArgList(ial, workDir);

  // TODO:
  // ComputeOutputOptions(toolChain, *dal, inputs, driverOpts, batchMode);

  // Get the compilation mode
  auto compilationModelKind = ComputeCompilationModelKind(*dal);

  auto compilationModel = ComputeCompilationModel(compilationModelKind);

  auto compilation = compilationModel->BuildCompilation(
      toolChain, inputs, GetDriverOptions().outputOptions);

  // A quick -print-requests check
  if (GetDriverOptions().printJobs) {
    compilation->PrintJobs();
    compilation.reset();
  }
  return compilation;
}
/// We take the Jobs from the compilation and we create TaskDetails that
///  are used to add Tasks into the TaskQueue
/// auto taskDetail = job.ToTaskDetail() => tq.CreateTask(taskDetail)
std::unique_ptr<TaskQueue>
Driver::BuildTaskQueue(const Compilation &compilation) {
  return nullptr;
}

void Driver::ComputeLinkMode(const llvm::opt::InputArgList &ial) {

  switch (GetDriverOptions().GetAction().GetKind()) {
  case ActionKind::None:
    GetDriverOptions().outputOptions.linkMode = LinkMode::EmitExecutable;
    break;
  case ActionKind::EmitLibrary: {
    if (ial.hasArg(opts::Static)) {
      GetDriverOptions().outputOptions.linkMode = LinkMode::EmitStaticLibrary;
      break;
    }
    GetDriverOptions().outputOptions.linkMode = LinkMode::EmitDynamicLibrary;
    break;
  }
  default:
    stone::Panic("Alien mode");
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

CompilationModelKind
Driver::ComputeCompilationModelKind(const llvm::opt::DerivedArgList &dal) {
  // TODO: Just Quad for now
  return CompilationModelKind::Quadratic;
}

// TODO:
std::unique_ptr<CompilationModel>
Driver::ComputeCompilationModel(CompilationModelKind kind) {
  switch (kind) {
  case CompilationModelKind::Quadratic:
    return std::make_unique<QuadraticCompilationModel>();
  // case CompilationMode::Flat:
  //   return std::make_unique<FlatCompilationModel>();
  // case CompilationMode::CPU:
  //   return std::make_unique<CPUCompilationModel>();
  // case CompilationMode::Single:
  //   return std::make_unique<SingleCompilationModel>();
  default:
    stone::Panic("Unknown CompilationModel kind");
  }
  return nullptr;
}

std::unique_ptr<ToolChain>
Driver::BuildToolChain(const llvm::opt::InputArgList &argList) {
  if (const llvm::opt::Arg *arg = argList.getLastArg(opts::Target)) {
    ctx.GetLangOptions().SetTarget(llvm::Triple::normalize(arg->getValue()));
  }

  switch (ctx.GetLangOptions().Target.getOS()) {
  case llvm::Triple::Darwin:
  case llvm::Triple::MacOSX: {
    llvm::Optional<llvm::Triple> targetVariant;
    if (const llvm::opt::Arg *A = argList.getLastArg(opts::TargetVariant)) {
      targetVariant = llvm::Triple(llvm::Triple::normalize(A->getValue()));
    }
    return std::make_unique<stone::darwin::DarwinToolChain>(
        *this, ctx.GetLangOptions().Target, targetVariant);
  }
  // case llvm::Triple::Linux:
  //   toolChain = std::make_unique<stone::linux::LinuxToolChain>(*this,
  //   target); break;
  // case llvm::Triple::FreeBSD:
  //   toolChain = std::make_unique<stone:unix::FreeBSDToolChain>(*this,
  //   target); break;
  // case llvm::Triple::OpenBSD:
  //   toolChain = std::make_unique<stone::unix::OpenBSDToolChain>(*this,
  //   target); break;
  // case llvm::Triple::Win32:
  //   toolChain = std::make_unique<stone::win::WinToolChain>(*this, target);
  //   break;
  default:
    assert(false && "OS not found!");
  }
}

llvm::StringRef Driver::ComputeOutputFilename() {}

// int Driver::ExecuteJob(const Job &job, const Job *&fallBackJob) const
// {
//   return 0;
// }
// void Driver::ExecuteJobs(
//     llvm::SmallVectorImpl<std::pair<int, const Job *>> &fallBackJob) const
//     {}

// void HotCache::Finish(Compilation &compilation,
//                       const OutputOptions &outputOpts) {

//  // Now, do we need any top-level JobRequests
// if (outputOptions.CanLink() && hc.ForLink()) {

//   Request *linkRequest = nullptr;
//   switch (GetLinkMode()) {
//   case LinkMode::EmitExecutable: {
//     linkRequest =
//         MakeJobAction<LinkJobRequest>(hc.forLink, GetLinkMode(), false);
//     break;
//   }
//   case LinkMode::EmitDynamicLibrary: {
//     linkRequest = MakeJobAction<LinkJobRequest>(hc.forLink, GetLinkMode(),
//                                               outputOptions.RequiresLTO());
//     break;
//   }
//   case LinkMode::EmitStaticLibrary: {
//     linkRequest =
//         MakeJobAction<LinkJobRequest>(hc.forLink, GetLinkMode(), false);
//     break;
//   }
//   default:
//     stone::Panic("Invalid linking mode");
//   }
//   assert(linkRequest);
//   hc.AddTopLevelRequest(linkRequest);
// }
//}
void Driver::PrintHelp(const llvm::opt::OptTable &opts) {}
void Driver::PrintVersion() {}
void Driver::Finish() {}
