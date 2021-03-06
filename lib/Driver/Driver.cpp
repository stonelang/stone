#include "stone/Driver/Driver.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/Ret.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/ToolChain.h"
#include "stone/Session/ModeType.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/Support/FormatVariadic.h"

using namespace stone;
using namespace stone::file;
using namespace stone::driver;
using namespace llvm::opt;

class DriverInternal final {
public:
  static bool DoesInputExist(Driver &driver, const DerivedArgList &args,
                             llvm::StringRef input);

public:
  /// Print the job
  static void PrintJob(const Job &job, Driver &driver);

  /// Print the job
  static void PrintJobVerbosely(Job *job, Driver &driver);

  /// Build jobs for multiple compiles -- each job gets one source file
  static void BuildJobsForMultipleCompileType(Driver &driver);

  /// Build jobs for a single compile -- the compile jobs has multiple files.
  static void BuildJobsForSingleCompileType(Driver &driver);

  // TODO:
  static void ComputeCompileType(const Driver &driver,
                                 const DerivedArgList &args,
                                 const Files &inputs);

  static void ComputeCompilerOutputFile(const Driver &driver);

  static llvm::StringRef ComputeBaseNameForImage(const Job *job,
                                                 const OutputProfile &op,
                                                 const llvm::Triple &triple,
                                                 llvm::SmallString<128> &buffer,
                                                 llvm::StringRef baseInput,
                                                 llvm::StringRef baseName);
  /*
  static llvm:::StringRef ComputeOutputFilename(Compilation &compilation,
                             const Job *job,
                             const TypeToPathMap *OutputMap,
                             llvm::StringRef workingDir,
                             bool atTopLevel,
                             llvm::StringRef baseInput,
                             llvm::StringRef brimaryInput,
                             llvm::SmallString<128> &buffer);
  */

  static std::unique_ptr<driver::TaskQueue> BuildTaskQueue(Driver &driver);

public:
  /// Builds the compile jobs
  static void BuildCompileJobs(Driver &driver);

  /// Builds the  link job
  static void BuildLinkJob(Driver &driver);

  /// Build compile only jobs
  static void BuildBackendJob(Driver &driver);

  /// Build compile only jobs
  static void BuildAssembleJob(Driver &driver);
};

/// Check that the file referenced by \p Input exists. If it doesn't,
/// issue a diagnostic and return false.
bool DriverInternal::DoesInputExist(Driver &driver, const DerivedArgList &args,
                                    llvm::StringRef input) {
  if (!driver.GetCheckInputFilesExist()) {
    return true;
  }
  // stdin always exists.
  if (input == "-") {
    return true;
  }
  if (file::Exists(input)) {
    return true;
  }
  driver.Out() << "de.D(SourceLoc(),"
               << "diag::error_no_such_file_or_directory, Input);" << input
               << '\n';
  return false;
}
void DriverInternal::BuildCompileJobs(Driver &driver) {
  if (!driver.GetMode().IsCompilable()) {
    return;
  }
  if (driver.GetInputFiles().size() == 0) {
    return;
  }
  OutputFileMap fileMap;
  auto tool = driver.GetToolChain().PickTool(JobType::Compile);
  assert(tool && "Could not find a tool for CompileJob.");

  auto cmdOutput = std::make_unique<CmdOutput>("todo", fileMap);

  for (auto &input : driver.GetInputFiles()) {
    if (input.GetType() == Type::Stone) {
      assert(file::IsPartOfCompilation(input.GetType()));

      auto job = tool->CreateJob(driver.GetCompilation(), std::move(cmdOutput),
                                 driver.GetOutputProfile());

      if (driver.GetMode().IsCompileOnly()) {
        driver.GetCompilation().AddJob(job);
      }
    }
  }
}

void DriverInternal::BuildLinkJob(Driver &driver) {
  if (driver.GetMode().IsCompileOnly()) {
    return;
  }
  if (!driver.GetOutputProfile().RequiresLink()) {
    return;
  }
  if (driver.GetMode().IsLinkOnly()) {
    for (auto &input : driver.GetInputFiles()) {
      switch (input.GetType()) {
      case Type::Object:
        break;
      default:
        break;
      }
    }
    return;
  }

  BuildCompileJobs(driver);

  if (driver.GetMode().IsLinkable()) {
    Job *linkJob = nullptr;
    switch (driver.GetOutputProfile().linkType) {
    case LinkType::StaticLibrary: {
      // TODO: This makes the most sense
      // driver.GetToolChain().PickTool(JobType::StaticLink).CreateJob();
      // linkJob = driver.GetCompilation().CreateJob<StaticLinkJob>(
      //    driver.GetCompilation(), driver.GetOutputProfile().RequiresLTO(),
      //    driver.GetOutputProfile().linkType);
      break;
    }
    case LinkType::DynamicLibrary: {
      // driver.GetToolChain().PickTool(JobType::DynamicLink).CreateJob();
      // linkJob = driver.GetCompilation().CreateJob<DynamicLinkJob>(
      //    driver.GetCompilation(), driver.GetOutputProfile().RequiresLTO(),
      //    driver.GetOutputProfile().linkType);
      // TODO: get the tool from the ToolChain and pass to CreateJob?
      break;
    }
    default:
      break;
    }
    assert(linkJob && "LinkJob was not created -- requires linking.");
    /*
for (auto job : internal.jobs) {
if (job->GetType() == JobType::Compile) {
linkJob->AddDep(job);
}
}
    */

    assert(linkJob && "LinkJob was not created.");
    // TODO: Since this is top level, directly add to
    // driver.GetCompilation().AddJob()
    // internal.TableJob(linkJob);
    // TODO: Move to driver.GetCompilation().AddJob()
  }
}

void DriverInternal::BuildAssembleJob(Driver &driver) {}

void DriverInternal::BuildBackendJob(Driver &driver) {}

void DriverInternal::BuildJobsForMultipleCompileType(Driver &driver) {
  if (driver.GetMode().IsCompileOnly()) {
    BuildCompileJobs(driver);
    return;
  }
  if (driver.GetOutputProfile().RequiresLink()) {
    BuildLinkJob(driver);
    return;
  }
}
void DriverInternal::BuildJobsForSingleCompileType(Driver &driver) {
  /*
    auto job =
        driver.GetCompilation().CreateJob<CompileJob>(driver.GetCompilation());

    for (const auto &input : driver.GetInputFiles()) {
      switch (input.GetType()) {
        case file::Type::Stone: {
          assert(file::IsPartOfCompilation(input.GetType()));
          job->AddInput(input);
          break;
        }
        default:
          break;
      }
    }
          */

  // job->BuildCmdOutput();
}

void DriverInternal::ComputeCompileType(const Driver &driver,
                                        const DerivedArgList &args,
                                        const Files &inputs) {}

DriverStats::DriverStats(const Driver &driver, Basic &basic)
    : Stats("driver statistics:", basic), driver(driver) {}

void DriverStats::Print() {
  if (driver.driverOpts.printStats) {
    GetBasic().Out() << GetName() << '\n';
    return;
  }
}

Driver::Driver(llvm::StringRef stoneExecutable, llvm::StringRef driverName)
    : Session(driverOpts, SessionType::Driver),
      stoneExecutablePath(stoneExecutablePath), driverName(driverName),
      /*sysRoot(DEFAULT_SYSROOT),*/
      driverTitle("Compiler driver"), checkInputFilesExist(true) {

  stats.reset(new DriverStats(*this, *this));
  GetStatEngine().Register(stats.get());
}

std::unique_ptr<driver::TaskQueue>
DriverInternal::BuildTaskQueue(Driver &driver) {
  // TODO:
  return std::make_unique<driver::UnixTaskQueue>(driver);
}

void Driver::Init() {}
/// Parse the given list of strings into an InputArgList.
bool Driver::Build(llvm::ArrayRef<const char *> args) {

  excludedFlagsBitmask = opts::NoDriverOption;

  originalArgs = ParseArgList(args);
  // TODO: Check for errors
  BuildCompilation(*originalArgs);
  if (de.HasError()) {
    return false;
  }
  return true;
}

void Driver::BuildToolChain(const llvm::opt::InputArgList &argList) {
  if (const llvm::opt::Arg *arg = argList.getLastArg(opts::Target)) {
    targetTriple = llvm::Triple::normalize(arg->getValue());
  }
  llvm::Triple target(targetTriple);

  switch (target.getOS()) {
  case llvm::Triple::Darwin:
  case llvm::Triple::MacOSX: {
    llvm::Optional<llvm::Triple> targetVariant;
    if (const llvm::opt::Arg *A = argList.getLastArg(opts::TargetVariant)) {
      targetVariant = llvm::Triple(llvm::Triple::normalize(A->getValue()));
    }
    toolChain = std::make_unique<DarwinToolChain>(*this, target, targetVariant);
    toolChain->Build();
    break;
  }
    /*
        case llvm::Triple::Linux:
          toolChain = std::make_unique<stone::Linux>(*this, target);
          break;
        case llvm::Triple::FreeBSD:
          toolChain = std::make_unique<stone::FreeBSD>(*this, target);
          break;
        case llvm::Triple::OpenBSD:
          toolChain = std::make_unique<stone::OpenBSD>(*this, target);
          break;
        case llvm::Triple::Win32:
          toolChain = std::make_unique<stone::Win>(*this, target);
          break;
    */
  default:
    Out() << "D(SourceLoc(),"
          << "msg::error_unknown_target,"
          << "ArgList.getLastArg(opts::Target)->getValue());" << '\n';
    break;
  }
}
void Driver::BuildCompilation(const llvm::opt::InputArgList &argList) {
  llvm::PrettyStackTraceString CrashInfo("Compilation construction");

  // TODO:
  // workingDir = ComputeWorkingDir(argList.get());

  // NOTE: Session manages this object
  translatedArgs = TranslateArgList(argList);

  CreateTimer();

  // Computer the compiler mode.
  ComputeMode(*translatedArgs);

  if (HasError())
    return;

  BuildToolChain(*originalArgs);
  // TODO: Check for errors

  // Perform toolchain specific args validation.
  // toolChain.ValidateArguments(de, *dArgList, targetTriple);
  //
  if (EmitInfo(*translatedArgs, GetToolChain())) {
    return;
  }

  BuildInputs(*translatedArgs, GetInputFiles());

  if (HasError())
    return;

  if (GetInputFiles().size() == 0) {
    Out() << "msg::driver_error_no_input_files" << '\n';
    return;
  }

  BuildOutputProfile(*translatedArgs, GetOutputProfile());

  if (HasError())
    return;

  // TODO: ComputeCompileMod()
  //
  // About to move argument list, so capture some flags that will be needed

  driverOpts.printJobs = translatedArgs->hasArg(opts::PrintDriverJobs);
  driverOpts.printLifecycle =
      translatedArgs->hasArg(opts::PrintDriverLifecycle);
  driverOpts.printStats = translatedArgs->hasArg(opts::PrintStats);

  compilation = std::make_unique<Compilation>(*this);

  BuildJobs();

  if (HasError())
    return;

  if (driverOpts.printJobs) {
    PrintJobs();
  }
}
void Driver::BuildOptions() {}

bool Driver::EmitInfo(const ArgList &args, const ToolChain &tc) {
  if (args.hasArg(opts::Help)) {
    PrintHelp(false);
    return true;
  }
  if (args.hasArg(opts::Version)) {
    PrintVersion();
    return true;
  }
  return false;
}

void Driver::BuildOutputProfile(const llvm::opt::DerivedArgList &args,
                                OutputProfile &outputProfile) const {
  auto compilerOutputType =
      outputProfile.ltoVariant != LTOKind::None ? Type::BC : Type::Object;
  // By default, the driver does not link its outputProfile. this will be
  // updated appropriately below if linking is required.
  //
  outputProfile.compilerOutputType = compilerOutputType;

  // if (const Arg *numThreads = args.getLastArg(opts::NumThreads)) {
  // if (StringRef(A->getValue()).getAsInteger(10, outputProfile.numThreads)) {
  // Diags.diagnose(SourceLoc(), diag::error_invalid_arg_value,
  //               A->getAsString(Args), A->getValue());
  //}
  //}

  // Basic for the time being
  switch (mode.GetType()) {
  case ModeType::EmitLibrary: {
    outputProfile.linkType = args.hasArg(opts::Static)
                                 ? LinkType::StaticLibrary
                                 : LinkType::DynamicLibrary;
    outputProfile.compilerOutputType = compilerOutputType;
  } break;
  case ModeType::EmitObject:
    outputProfile.compilerOutputType = Type::Object;
    break;
  case ModeType::EmitAssembly:
    outputProfile.compilerOutputType = Type::Assembly;
    break;
  case ModeType::EmitIR:
    outputProfile.compilerOutputType = Type::IR;
    break;
  case ModeType::EmitBC:
    outputProfile.compilerOutputType = Type::BC;
    break;
  case ModeType::Parse:
  case ModeType::TypeCheck:
    outputProfile.compilerOutputType = Type::None;
    break;
  default:
    outputProfile.linkType = LinkType::Executable;
    break;
  }
  assert(outputProfile.compilerOutputType != Type::INVALID);
}

ModeType Driver::GetDefaultModeType() { return ModeType::EmitExecutable; }

void Driver::ComputeMode(const llvm::opt::DerivedArgList &args) {
  Session::ComputeMode(args);
}

void Driver::PrintJobs() {
  for (auto &job : GetCompilation().GetJobs()) {
    DriverInternal::PrintJob(job, *this);
  }
}

void Driver::PrintLifecycle() {}

void Driver::PrintHelp(bool showHidden) {
  excludedFlagsBitmask = opts::NoDriverOption;
  // if (!showHidden)
  //  excludedFlagsBitmask |= HelpHidden;

  driverOpts.GetOpts().PrintHelp(Out().GetOS(), driverName.c_str(),
                                 "Stone Compiler", includedFlagsBitmask,
                                 excludedFlagsBitmask,
                                 /*ShowAllAliases*/ false);
}

void DriverInternal::PrintJob(const Job &job, Driver &driver) {
  auto cos = driver.Out();
  cos.UseGreen();

  if (job.GetDeps().size() > 0) {
    for (const auto &j : job.GetDeps()) {
      DriverInternal::PrintJob(j, driver);
    }
  }

  /*
      switch (job.GetType()) {
        case JobType::Compile: {
          auto *cj = dyn_cast<CompileJob>(job);
          cos << job.GetName() << '\n';
          cos.UseMagenta();
          for (auto input : job.GetJobOptions().inputs) {
            cos << ' ' << input.second << ' ' << "->" << ' '
                << job.GetOutputTypeName() << '\n';
          }
          break;
        }
        default:
          break;
      }
  */
}

void Driver::ComputeModuleOutputPath() {}
void Driver::ComputeCmdOutput() {}
void Driver::ComputeMainOutput() {}

void Driver::AddJobForCompilation(const Job *job) {
  GetCompilation().AddJob(job);
}

int Driver::Run() {
  // Perform a quick help check
  if (driverOpts.printHelp) {
    // PrintHelp();
  }
  // auto compilationResult =
  //	GetCompilation().Run(DriverInternal::BuildTaskQueue(*this));

  if (HasError())
    return ret::err;

  return ret::ok;
}
