#include "stone/Session/Session.h"
#include "stone/Diag/CoreDiagnostic.h"
#include "stone/Context.h"
#include "stone/Session/Options.h"
#include "llvm/Option/Option.h"

using namespace stone;
using namespace stone::opts;

using namespace llvm::opt;

Session::Session() : optst(stone::opts::CreateOptTable()) { CreateTimer(); }

Session::~Session() {}

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
    GetContext().GetDiagUnit().PrintD(SrcLoc(), diag::err_missing_arg_value,
                                      diag::LLVMStr(ial->getArgString(missingArgIndex)),
                                      diag::UInt(missingArgCount));
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : ial->filtered(opts::UNKNOWN)) {
    GetContext().GetDiagUnit().PrintD(SrcLoc(), diag::err_unknown_arg,
                                      diag::LLVMStr(arg->getAsString(*ial)));
  }
  return *ial.get();
}
Mode &Session::ComputeMode(const llvm::opt::InputArgList &ial) {
  mode = Mode::Create(ial);
  return *mode.get();
}

file::Files &Session::BuildInputFiles(const llvm::opt::InputArgList &ial) {
  llvm::DenseMap<llvm::StringRef, llvm::StringRef> seenFiles;
  unsigned fileID = 0;
  for (Arg *arg : ial) {
    if (arg->getOption().getKind() == llvm::opt::Option::InputClass) {
      auto input = arg->getValue();
      if (file::Exists(input)) {
        fileID++;
        auto fileType = file::GetTypeByExt(file::GetExt(input));
        switch (fileType) {
        case file::Type::Stone: {
          if (GetBaseOptions().inputFileType == file::Type::None) {
            GetBaseOptions().inputFileType = file::Type::Stone;
          } else if (GetBaseOptions().inputFileType != file::Type::Stone) {
            stone::Panic("Different file types"); // TODO: PrintD
          }
          AddInputFile(input, fileType, fileID);
          break;
        }
        case file::Type::Object: {
          if (GetBaseOptions().inputFileType == file::Type::None) {
            GetBaseOptions().inputFileType = file::Type::Object;
          } else if (GetBaseOptions().inputFileType != file::Type::Object) {
            // TODO: Different file types
            stone::Panic("Different file types"); // TODO: PrintD
          }
          AddInputFile(input, fileType, fileID);
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
void Session::AddInputFile(llvm::StringRef name, unsigned fileID) {
  auto ty = file::GetTypeByName(name);
  assert(ty != file::Type::INVALID && "Invalid file type.");
  AddInputFile(name, ty, fileID);
}
// TODO: There is a potential to add duplicate files here.
void Session::AddInputFile(llvm::StringRef name, file::Type ty,
                           unsigned fileID) {
  GetBaseOptions().inputFiles.push_back(file::File(name, ty, fileID));
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

void Session::PrintVersion() {}
void Session::PrintTimer() {}
void Session::PrintDiagnostics() {}
void Session::PrintStatistics() {}
