#include "stone/Driver/Driver.h"
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

Driver::Driver(llvm::StringRef name, llvm::StringRef path,
               CompilationListener *listener)
    : optUtil(GetDriverOptions()) {

  buildSystem = std::make_unique<BuildSystem>(*this);
}

Driver::~Driver() {}

void Driver::Initialize() {}

bool Driver::ParseArgs(llvm::ArrayRef<const char *> args) {

  GetOptUtil().SetExcludedFlagsBitmask(opts::NoDriverOption);
  if (GetOptUtil().ParseArgs(args, &ctx) == stone::Err) {
    return stone::Err;
  }

  ComputeLinkMode(GetOptUtil().GetInputArgList());

  return stone::Ok;
}

std::unique_ptr<Compilation> Driver::BuildCompilation(ToolChain &tc) {

  if (driverOpts.cleanBuild) {
    GetBuildSystem().Clean();
  }
  GetBuildSystem().StartBuild();

  // Now, build the job system since we have a toolchain
  auto compilation =
      std::make_unique<Compilation>(GetContext(), tc, GetBuildSystem());

  BuildCompilationState bcs;
  BuildIntents(*compilation.get(), bcs);

  if (driverOpts.printIntents) {
    // PrintIntents(bi);
    return nullptr;
  }

  // BuiltJobs bj;
  // BuildJobs(bj, bi);

  // return compilation;

  return nullptr;
}

void Driver::ComputeLinkMode(const llvm::opt::InputArgList &ial) {

  switch (GetMode().GetKind()) {
  case ModeKind::None: {
    GetDriverOptions().linkMode = LinkMode::EmitExecutable;
    break;
  }
  case ModeKind::EmitLibrary: {
    if (ial.hasArg(opts::Static)) {
      GetDriverOptions().linkMode = LinkMode::EmitStaticLibrary;
    } else {
      GetDriverOptions().linkMode = LinkMode::EmitDynamicLibrary;
    }
    break;
  }
  case ModeKind::Alien: {
    // ctx.Printd(err_alien_mode);
    break;
  }
  default: {
    assert(GetMode().CanCompile() && "Invalid compile mode.");
    break;
  }
  }
}

stone::CompileModel
Driver::ComputeCompileModel(const llvm::opt::DerivedArgList &args,
                            const file::Files &inputs) const {
  // Just use multiple for now
  return stone::CompileModel::Multiple;
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

void Driver::BuildDriverOptions(const llvm::opt::InputArgList &ial) {

  // Since the mode has already been created
  // switch(GetMode().GetKind().)
  driverOpts.outputFileType = file::Type::Object;

  // TODO:
  // driverOpts.compileModel = ComputeCompileModel(
  //     *tal, driverOpts.inputFiles);

  // auto scPathResult = GetEQValue(opts::LangPathEQ);
  // if (!stPathResult.IsErr()) {
  //   driverOpts.scPath = stPathResult.Get();
  // }

  driverOpts.printIntents = ial.hasArg(opts::PrintDriverIntents);
  driverOpts.printJobs = ial.hasArg(opts::PrintDriverJobs);
  driverOpts.printLifecycle = ial.hasArg(opts::PrintDriverLifecycle);
  driverOpts.systemOpts.printStatistics = ial.hasArg(opts::PrintStats);
}

// IntentExecutor Driver::ConstructIntentExecutor(CompilationIntent &intent) {
//   // CommandIntentExecutor(intent, .... tc.ContructInvocation(intent));
// }

llvm::StringRef Driver::ComputeOutputFilename() {}

// int Driver::ExecuteJob(const Job &job, const Job *&fallBackJob) const
// {
//   return 0;
// }
// void Driver::ExecuteJobs(
//     llvm::SmallVectorImpl<std::pair<int, const Job *>> &fallBackJob) const
//     {}

void Driver::PrintHelp() {}

void Driver::PrintVersion() {}
void Driver::Finish() {}
