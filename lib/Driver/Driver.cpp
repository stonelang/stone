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
               CompilationListener *listener) {

  buildSystem = std::make_unique<BuildSystem>(*this);
}

Driver::~Driver() {}

void Driver::Initialize() {}

bool Driver::ParseArgs(llvm::ArrayRef<const char *> args) {

  optSupport.SetExcludedFlagsBitmask(opts::NoDriverOption);
  if (optSupport.ParseArgs(args, &ctx) == stone::Err) {
    return stone::Err;
  }
  // support.BuildInputFiles(GetInputArgList());
  ComputeLinkMode();

  return stone::Ok;
}

std::unique_ptr<Compilation> Driver::BuildCompilation(ToolChain& tc) {

  // if (driverOpts.cleanBuild) {
  //   buildSystem->Clean();
  // }

  // Now, build the job system since we have a toolchain
  auto compilation = std::make_unique<Compilation>(*this);

  // Driver::Inflight inflight;

  // BuiltIntents bi;
  // BuildIntents(bi);

  if (driverOpts.printIntents) {
    // PrintIntents(bi);
    return;
  }

  // BuiltJobs bj;
  // BuildJobs(bj, bi);

  return compilation;
}

void Driver::ComputeLinkMode() {

  switch (GetMode().GetKind()) {
  case ModeKind::None: {
    GetDriverOptions().linkMode = LinkMode::EmitExecutable;
    break;
  }
  case ModeKind::EmitLibrary: {
    if (tal->hasArg(opts::Static)) {
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

void Driver::BuildToolChain(const llvm::opt::InputArgList &argList) {

  if (const llvm::opt::Arg *arg = argList.getLastArg(opts::Target)) {
    defaultTargetTriple = llvm::Triple::normalize(arg->getValue());
  }
  llvm::Triple target(defaultTargetTriple);

  switch (target.getOS()) {
  case llvm::Triple::Darwin:
  case llvm::Triple::MacOSX: {
    llvm::Optional<llvm::Triple> targetVariant;
    if (const llvm::opt::Arg *A = argList.getLastArg(opts::TargetVariant)) {
      targetVariant = llvm::Triple(llvm::Triple::normalize(A->getValue()));
    }
    toolChain = std::make_unique<DarwinToolChain>(*this, target, targetVariant);
    toolChain->Initialize();
    break;
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

void Driver::BuildOptions() {

  // We default to object files -- this will be updated as we go along
  driverOpts.outputFileType = file::Type::Object;

  // TODO:
  // driverOpts.compileModelKind = ComputeCompileModel(
  //     *tal, driverOpts.inputFiles);

  auto stcPathResult = GetEQValue(opts::LangPathEQ);
  if (!stcPathResult.IsErr()) {
    driverOpts.hasLangPath = true;
    driverOpts.stcPath = stcPathResult.Get();
  }

  driverOpts.printIntents = tal->hasArg(opts::PrintDriverIntents);
  driverOpts.printJobs = tal->hasArg(opts::PrintDriverJobs);
  driverOpts.printLifecycle = tal->hasArg(opts::PrintDriverLifecycle);
  driverOpts.systemOpts.printStatistics = tal->hasArg(opts::PrintStats);
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

void Driver::Finish() {}
