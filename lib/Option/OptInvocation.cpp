#include "stone/Option/OptInvocation.h"
#include "stone/Core/Context.h"
#include "stone/Option/Options.h"

#include "llvm/Option/Option.h"

using namespace stone;
using namespace stone::opts;

using namespace llvm::opt;

OptInvocation::OptInvocation() : optst(stone::opts::CreateOptTable()) {}

llvm::opt::InputArgList &
OptInvocation::ParseArgs(llvm::ArrayRef<const char *> args) {

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
Mode &OptInvocation::CreateMode(const llvm::opt::InputArgList &ial) {

  auto modeArg = ial.getLastArg(opts::ModeGroup);
  if (modeArg) {
    // TODO: may have to claim
    switch (modeArg->getOption().getID()) {
    case opts::Parse:
      mode = std::make_unique<Mode>(ModeKind::Parse);
      break;
    case opts::EmitParse:
      mode = std::make_unique<Mode>(ModeKind::EmitParse);
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

file::Files &
OptInvocation::BuildInputFiles(const llvm::opt::InputArgList &ial) {

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
void OptInvocation::AddInputFile(llvm::StringRef name) {
  auto ty = file::GetTypeByName(name);
  assert(ty != file::Type::INVALID && "Invalid file type.");
  AddInputFile(name, ty);
}
// TODO: There is a potential to add duplicate files here.
void OptInvocation::AddInputFile(llvm::StringRef name, file::Type ty) {
  GetBaseOptions().inputFiles.push_back(file::File(name, ty));
}

void OptInvocation::PrintHelp() {}

// void Support::BuildInputFiles() {}
