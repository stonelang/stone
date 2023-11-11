//#include "stone/Compile/Compiler.h"

// using namespace stone;

// CompilerAction::CompilerAction() {}

// CompilerCommandLine::CompilerCommandLine()
//     : compilerOptionTable(opts::CreateOptTable()) {}

// Status CompilerCommandLine::ParseCompilerAction(CompilerInputArgList &args) {
//   auto actionArg = args.GetActionArg();
//   if (actionArg) {
//     auto actionKind = action::GetActionKindByOptionID(args.GetArgID(actionArg));
//     compilerAction.SetKind(actionKind);
//     if (compilerAction.IsAlien()) {
//       return Status::Error();
//     }
//     compilerAction.SetName(args.GetArgName(actionArg));
//   }
//   return Status();
// }

// Status CompilerCommandLine::ParseCompilerOptions(CompilerInputArgList &args) {

//   // CompilerOptionsConverter converter(GetBasic().GetDiags(), ial,
//   //                                    GetLangOptions(), GetCompilerOptions(),
//   //                                    GetModuleOptions());

//   // CompilerOptionsConverter converter(*this);

//   return Status();
// }

// Status CompilerCommandLine::ParseCodeGenOptions() { return Status(); }

// Status CompilerCommandLine::ParseASTOptions() { return Status(); }

// Status CompilerCommandLine::ParseSearchPathOptions() { return Status(); }

// Status CompilerCommandLine::ParseTypeCheckerOptions() { return Status(); }

// Status CompilerCommandLine::ParseModuleOptions() { return Status(); }

// Status
// CompilerCommandLine::ParseCommandLine(llvm::ArrayRef<const char *> args) {

//   auto argList = std::make_unique<llvm::opt::InputArgList>(
//       GetCompilerOptionTable().ParseArgs(
//           args, flags.missingArgIndex, flags.missingArgCount,
//           flags.includedFlagsBitmask, flags.excludedFlagsBitmask));

//   assert(argList && "No input argument list.");
//   CompilerInputArgList compilerInputArgList(*argList);

//   auto status = ParseCompilerAction(compilerInputArgList);
//   if (satus.IsError()) {
//     satus.SetHasCompletion();
//     return status;
//   }
//   return Status();
// }