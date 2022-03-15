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

Driver::Driver(const char *programName, const char *programPath)
    : Session(SessionKind::Driver, programName, programPath) {

  buildSystem = std::make_unique<BuildSystem>(*this);
}

void Driver::Initialize() {
  excludedFlagsBitmask = opts::NoDriverOption;
  Session::Initialize();
}

Driver::~Driver() {}

// llvm::opt::InputArgList &
// Driver::ParseArguments(llvm::ArrayRef<const char *> args){

//   return Session::ParseArguments(args);
// }
// Build the session
void Driver::BuildSession(const llvm::opt::InputArgList &ial) {}

void Driver::BuildCompilation() {

  if (driverOpts.cleanBuild) {
    buildSystem->Clean();
  }
  // Now, build the job system since we have a toolchain
  compilation = std::make_unique<Compilation>(*this);

  // Driver::Inflight inflight;

  // BuiltIntents bi;
  // BuildIntents(bi);

  if (driverOpts.printIntents) {
    // PrintIntents(bi);
    return;
  }

  // BuiltJobs bj;
  // BuildJobs(bj, bi);
}

struct Driver::Inflight final {

  /// All the inputs associated with the module
  llvm::SmallVector<const Intent *, 4> moduleInputs;

  /// The top level intents -- ex: linker. We only queue the top level intents.
  llvm::SmallVector<const Intent *, 16> topLevelIntents;

  /// All the inputs for the linker
  llvm::SmallVector<const Intent *, 2> linkerInputs;

  Intent *current;
};

void Driver::ComputeLinkKind() {

  switch (GetMode().GetKind()) {
  case ModeKind::Unknown: {
    // record error
    break;
  }
  case ModeKind::None: {
    GetDriverOptions().linkKind = LinkKind::EmitExecutable;
    break;
  }
  case ModeKind::EmitLibrary: {
    if (tal->hasArg(opts::Static)) {
      GetDriverOptions().linkKind = LinkKind::EmitStaticLibrary;
    } else {
      GetDriverOptions().linkKind = LinkKind::EmitDynamicLibrary;
    }
    break;
  }
  default: {
    assert(GetMode().CanCompile() && "Invalid compile mode.");
    break;
  }
  }
}

void Driver::Build(llvm::ArrayRef<const char *> args) {

  Session::Build(args);

  ComputeLinkKind();

  BuildToolChain(*ial);

  BuildCompilation();
}
stone::CompileModelKind
Driver::ComputeCompileModelKind(const llvm::opt::DerivedArgList &args,
                                const file::Files &inputs) const {
  // Just use multiple for now
  return stone::CompileModelKind::Multiple;
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
  // driverOpts.compileModelKind = ComputeCompileModelKind(
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

ModeKind Driver::GetDefaultMode() { return ModeKind::None; }

llvm::StringRef Driver::GetSessionName() { return "Stone"; }
llvm::StringRef Driver::GetSessionDesc() { return "Stone compilation time"; }

void Driver::Finish() { Session::Finish(); }
