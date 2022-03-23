#include "stone/Driver/Driver.h"
#include "stone/Core/CoreDiagnostic.h"
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

  if (inputs.empty()) {
    GetContext().Printd(SrcLoc(), diag::err_no_input_files);
    return nullptr;
  }

  auto workDir = ComputeWorkDir(ial);

  /// TODO: for now...move into ComputeOptions
  ComputeLinkMode(ial);

  auto dal = TranslateInputArgList(ial, workDir);

  // TODO:
  // ComputeOutputOptions(toolChain, *dal, inputs, driverOpts, batchMode);

  // Get the compilation mode
  auto compilationMode = ComputeCompilationMode(*dal);

  auto compilationModel = ComputeCompilationModel(compilationMode);

  auto compilation = compilationModel->BuildCompilation(
      *this, toolChain, inputs, driverOpts.outputOptions);

  // A quick -print-requests check
  if (driverOpts.printJobs) {
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

  switch (GetMode().GetKind()) {
  case ModeKind::None:
    GetDriverOptions().outputOptions.linkMode = LinkMode::EmitExecutable;
    break;
  case ModeKind::EmitLibrary: {
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

CompilationMode
Driver::ComputeCompilationMode(const llvm::opt::DerivedArgList &dal) {
  // Just Quad for now
  return CompilationMode::Quadratic;
}

std::unique_ptr<CompilationModel>
Driver::ComputeCompilationModel(CompilationMode mode) {
  switch (mode) {
  case CompilationMode::Quadratic:
    return std::make_unique<QuadraticCompilationModel>();
  // case CompilationMode::Flat:
  //   return std::make_unique<FlatCompilationModel>();
  // case CompilationMode::CPU:
  //   return std::make_unique<CPUCompilationModel>();
  // case CompilationMode::Single:
  //   return std::make_unique<SingleCompilationModel>();
  default:
    stone::Panic("Unknown Compilation Mode");
  }
  return nullptr;
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
    return std::make_unique<stone::darwin::DarwinToolChain>(
        *this, ctx.GetSystemOptions().target, targetVariant);
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
    stone::Panic("OS not found!");
  }
}

void Driver::ComputeOptions(const llvm::opt::InputArgList &ial) {
  // Since the mode has already been created
  // switch(GetMode().GetKind().)
  driverOpts.outputFileType = file::Type::Object;

  // TODO:
  // driverOpts.compileModel = ComputeCompilationMode(
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

// void HotCache::Finish(Compilation &compilation,
//                       const OutputOptions &outputOpts) {

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
//}
void Driver::PrintVersion() {}
void Driver::Finish() {}
