#include "stone/Option/OptUtil.h"
#include "stone/Core/Context.h"

#include "stone/Option/Options.h"

using namespace stone;
using namespace stone::opts;

OptUtil::OptUtil() : optst(stone::opts::CreateOptTable()) {}

bool OptUtil::ParseArgs(llvm::ArrayRef<const char *> args, Context *ctx) {

  ial = std::make_unique<llvm::opt::InputArgList>(
      GetOpts().ParseArgs(args, missingArgIndex, missingArgCount,
                          includedFlagsBitmask, excludedFlagsBitmask));

  assert(ial && "No input argument list.");

  // Check for missing argument error.
  if (missingArgCount && ctx) {
    // TODO:
    // ctx.Printd << "D(SrcLoc(),"
    //     << "msg::error_missing_arg_value,"
    //     << "argList->getArgString(missingArgIndex),"
    //     << "missingArgCount" << '\n';
    /// TODO: return stone::Err
    stone::Panic("error_missing_arg_value");
  }

  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : ial->filtered(opts::UNKNOWN)) {
    /// TODO: ctx.Printd
    // cos << "D(SourceLoc(), "
    //     << "msg::error_unknown_arg,"
    //     << "arg->getAsString(*ArgList));" << '\n';
    // TODO: return stone::Err
    stone::Panic("error_unknown_arg");
  }
  // Create the mode
  mode = OptUtil::CreateMode(GetInputArgList());

  // Build all input files
  inputFiles = OptUtil::BuildInputFiles(GetInputArgList());

  return stone::Ok;
}
std::unique_ptr<Mode> OptUtil::CreateMode(const llvm::opt::InputArgList &ial) {

  auto modeArg = ial.getLastArg(opts::ModeGroup);
  if (modeArg) {
    // TODO: may have to claim
    switch (modeArg->getOption().getID()) {
    case opts::Parse:
      return std::make_unique<Mode>(ModeKind::Parse);
    case opts::EmitParse:
      return std::make_unique<Mode>(ModeKind::EmitParse);
    case opts::TypeCheck:
      return std::make_unique<Mode>(ModeKind::TypeCheck);
    case opts::EmitSyntax:
      return std::make_unique<Mode>(ModeKind::EmitSyntax);
    case opts::EmitIR:
      return std::make_unique<Mode>(ModeKind::EmitIR);
    case opts::EmitBC:
      return std::make_unique<Mode>(ModeKind::EmitBC);
    case opts::EmitObject:
      return std::make_unique<Mode>(ModeKind::EmitObject);
    case opts::EmitAssembly:
      return std::make_unique<Mode>(ModeKind::EmitAssembly);
    case opts::EmitLibrary:
      return std::make_unique<Mode>(ModeKind::EmitLibrary);
    case opts::EmitModule:
      return std::make_unique<Mode>(ModeKind::EmitModule);
    case opts::PrintVersion:
      return std::make_unique<Mode>(ModeKind::PrintVersion);
    case opts::PrintHelp:
      return std::make_unique<Mode>(ModeKind::PrintVersion);
    default:
      return std::make_unique<Mode>(ModeKind::Alien);
    }
  }
  return std::make_unique<Mode>(ModeKind::None);
}

file::Files OptUtil::BuildInputFiles(const llvm::opt::InputArgList &ial) {
  file::Files files;

  return files;
}
// // TODO: May move to session
// stone::Files Support::BuildInputFiles(const llvm::opt::InputArgList &args) {
// bool isDone = false;
//   llvm::DenseMap<llvm::StringRef, llvm::StringRef> seenFiles;

//   for (Arg *arg : args) {
//     if (arg->getOption().getKind() == Option::InputClass) {
//       auto input = arg->getValue();
//       if (file::Exists(input)) {
//         auto fileType = file::GetTypeByExt(file::GetExt(input));

//         switch (fileType) {
//         case file::Type::Stone: {
//           if (GetOptions().inputFileType == file::Type::None) {
//             GetOptions().inputFileType = file::Type::Stone;
//           } else if (GetOptions().inputFileType != file::Type::Stone) {
//             // TODO: Different file types
//             return;
//           }
//           AddFile(input, fileType);
//           break;
//         }
//         case file::Type::Object: {
//           if (GetOptions().inputFileType == file::Type::None) {
//             GetOptions().inputFileType = file::Type::Object;
//           } else if (GetOptions().inputFileType != file::Type::Object) {
//             // TODO: Different file types
//             return;
//             break;
//           }
//           AddFile(input, fileType);
//           break;
//         }
//         default:
//           stone::Panic("Unknown file type");
//           break;
//         }
//         if (fileType == file::Type::Stone) {
//           auto baseName = llvm::sys::path::filename(input);

//           if (!seenFiles.insert({baseName, input}).second) {
//             stone::Panic("error_two_files_same_name");

//             // GetContext().Out()
//             //     << "de.D(SourceLoc(),"
//             //     << "diag::error_two_files_same_name,"
//             //     << "basename, seenFiles[basename], argValue);" << '\n';
//             // GetContext().Out() << " de.D(SourceLoc(), "
//             //                  << "diag::note_explain_two_files_"
//             //                     "same_name);"
//             //                  << '\n';
//           }
//         }
//       }
//     }
//   }
// }

// void Support::AddFile(llvm::StringRef name) {
//   auto ty = file::GetTypeByName(name);
//   assert(ty != file::Type::INVALID && "Invalid file type.");
//   AddFile(name, ty);
// }
// void Support::AddFile(llvm::StringRef name, file::Type ty) {
//   inputFiles.push_back(file::File(name, ty));
// }
void OptUtil::PrintHelp() {}

// void Support::BuildInputFiles() {}
