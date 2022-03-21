#include "stone/Session/Session.h"

#include "stone/Core/Context.h"
#include "stone/Session/Options.h"
#include "llvm/Option/Option.h"

using namespace stone;
using namespace stone::opts;

using namespace llvm::opt;

Session::Session()
    : optst(stone::opts::CreateOptTable()),
      vfs(llvm::vfs::getRealFileSystem()) {
  CreateTimer();
}

void Session::CreateTimer() {
  // timerGroup =
  //     std::make_unique<llvm::TimerGroup>(GetSessionName(), GetSessionDesc());
  // timer = std::make_unique<llvm::Timer>(GetSessionName(), GetSessionDesc(),
  //                                       *timerGroup);
  // timer->startTimer();
}

llvm::opt::InputArgList &Session::ParseArgs(llvm::ArrayRef<const char *> args) {
  ial = std::make_unique<llvm::opt::InputArgList>(
      GetOpts().ParseArgs(args, missingArgIndex, missingArgCount,
                          includedFlagsBitmask, excludedFlagsBitmask));

  assert(ial && "No input argument list.");

  // Check for missing argument error.
  if (missingArgCount) {
    // TODO:
    // GetContext().Printd << "D(SrcLoc(),"
    //     << "msg::error_missing_arg_value,"
    //     << "argList->getArgString(missingArgIndex),"
    //     << "missingArgCount" << '\n';
    /// TODO: return stone::Err
    stone::Panic("error_missing_arg_value");
  }

  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : ial->filtered(opts::UNKNOWN)) {
    /// TODO: GetContext().Printd
    // cos << "D(SourceLoc(), "
    //     << "msg::error_unknown_arg,"
    //     << "arg->getAsString(*ArgList));" << '\n';
    // TODO: return stone::Err
    stone::Panic("error_unknown_arg");
  }

  return *ial.get();
}
Mode &Session::ComputeMode(const llvm::opt::InputArgList &ial) {
  auto modeArg = ial.getLastArg(opts::ModeGroup);
  if (modeArg) {
    // TODO: may have to claim
    switch (modeArg->getOption().getID()) {
    case opts::Parse:
      mode = std::make_unique<Mode>(ModeKind::Parse);
      break;
    case opts::EmitParse:
      mode = std::make_unique<Mode>(ModeKind::EmitParse);
      break;
    case opts::TypeCheck:
      mode = std::make_unique<Mode>(ModeKind::TypeCheck);
      break;
    case opts::EmitSyntax:
      mode = std::make_unique<Mode>(ModeKind::EmitSyntax);
      break;
    case opts::EmitIR:
      mode = std::make_unique<Mode>(ModeKind::EmitIR);
      break;
    case opts::EmitBC:
      mode = std::make_unique<Mode>(ModeKind::EmitBC);
      break;
    case opts::EmitObject:
      mode = std::make_unique<Mode>(ModeKind::EmitObject);
      break;
    case opts::EmitAssembly:
      mode = std::make_unique<Mode>(ModeKind::EmitAssembly);
      break;
    case opts::EmitLibrary:
      mode = std::make_unique<Mode>(ModeKind::EmitLibrary);
      break;
    case opts::EmitModule:
      mode = std::make_unique<Mode>(ModeKind::EmitModule);
      break;
    case opts::PrintVersion:
      mode = std::make_unique<Mode>(ModeKind::PrintVersion);
      break;
    case opts::PrintHelp:
      mode = std::make_unique<Mode>(ModeKind::PrintVersion);
      break;
    default:
      mode = std::make_unique<Mode>(ModeKind::Alien);
      break;
    }
  }
  mode = std::make_unique<Mode>(ModeKind::None);
  return *mode.get();
}

file::Files &Session::BuildInputFiles(const llvm::opt::InputArgList &ial) {
  llvm::DenseMap<llvm::StringRef, llvm::StringRef> seenFiles;
  for (Arg *arg : ial) {
    if (arg->getOption().getKind() == Option::InputClass) {
      auto input = arg->getValue();
      if (file::Exists(input)) {
        auto fileType = file::GetTypeByExt(file::GetExt(input));
        switch (fileType) {
        case file::Type::Stone: {
          if (GetBaseOptions().inputFileType == file::Type::None) {
            GetBaseOptions().inputFileType = file::Type::Stone;
          } else if (GetBaseOptions().inputFileType != file::Type::Stone) {
            stone::Panic("Different file types"); // TODO: Printd
          }
          AddInputFile(input, fileType);
          break;
        }
        case file::Type::Object: {
          if (GetBaseOptions().inputFileType == file::Type::None) {
            GetBaseOptions().inputFileType = file::Type::Object;
          } else if (GetBaseOptions().inputFileType != file::Type::Object) {
            // TODO: Different file types
            stone::Panic("Different file types"); // TODO: Printd
          }
          AddInputFile(input, fileType);
          break;
        }
        default:
          stone::Panic("Unknown file type");
          break;
        }
        if (fileType == file::Type::Stone) {
          auto baseName = llvm::sys::path::filename(input);

          if (!seenFiles.insert({baseName, input}).second) {
            stone::Panic("error_two_files_same_name");

            // GetContext().Out()
            //     << "de.D(SourceLoc(),"
            //     << "diag::error_two_files_same_name,"
            //     << "basename, seenFiles[basename], argValue);" << '\n';
            // GetContext().Out() << " de.D(SourceLoc(), "
            //                  << "diag::note_explain_two_files_"
            //                     "same_name);"
            //                  << '\n';
          }
        }
      }
    }
  }
  return GetBaseOptions().inputFiles;
}
void Session::AddInputFile(llvm::StringRef name) {
  auto ty = file::GetTypeByName(name);
  assert(ty != file::Type::INVALID && "Invalid file type.");
  AddInputFile(name, ty);
}
// TODO: There is a potential to add duplicate files here.
void Session::AddInputFile(llvm::StringRef name, file::Type ty) {
  GetBaseOptions().inputFiles.push_back(file::File(name, ty));
}

llvm::StringRef Session::ComputeWorkDir(const llvm::opt::InputArgList &ial) {
  if (auto *arg = ial.getLastArg(opts::WorkDir)) {
    llvm::SmallString<128> smallStr;
    smallStr = arg->getValue();
    llvm::sys::fs::make_absolute(smallStr);
    return smallStr.str();
  }
  return llvm::StringRef();
}

stone::Result<std::string>
Session::GetOptEqualValue(opts::OptID optID,
                          const llvm::opt::InputArgList &ial) {
  if (ial.hasArg(optID)) {
    auto arg = ial.getLastArg(optID);
    if (arg) {
      return stone::Result<std::string>(arg->getValue());
    }
  }
  return stone::Result<std::string>();
}

void Session::PrintHelp(const llvm::opt::OptTable &optst) {}

void Session::PrintTimer() {}

void Session::PrintDiagnostics() {}

void Session::PrintStatistics() {}
