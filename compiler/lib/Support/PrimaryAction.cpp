// #include "stone/Support/PrimaryAction.h"
// #include "stone/Support/Options.h"

// using namespace stone;
// using namespace stone::file;

// bool Options::IsNoneAction() const {
//   return primaryAction == PrimaryAction::None;
// }

// bool Options::IsPrintHelpAction() const {
//   return primaryAction == PrimaryAction::PrintHelp;
// }
// bool Options::IsPrintHelpHiddenAction() const {
//   return primaryAction == PrimaryAction::PrintHelpHidden;
// }
// bool Options::IsPrintVersionAction() const {
//   return primaryAction == PrimaryAction::PrintVersion;
// }
// bool Options::IsPrintFeatureAction() const {
//   return primaryAction == PrimaryAction::PrintFeature;
// }
// bool Options::IsParseAction() const {
//   return primaryAction == PrimaryAction::Parse;
// }

// bool Options::IsResolveImportsAction() const {
//   return primaryAction == PrimaryAction::ResolveImports;
// }
// bool Options::IsPrintASTBeforeAction() const {
//   return primaryAction == PrimaryAction::PrintASTBefore;
// }
// bool Options::IsTypeCheckAction() const {
//   return primaryAction == PrimaryAction::TypeCheck;
// }
// bool Options::IsPrintASTAfterAction() const {
//   return primaryAction == PrimaryAction::PrintASTAfter;
// }
// bool Options::IsEmitIRAfterAction() const {
//   return primaryAction == PrimaryAction::EmitIRAfter;
// }
// bool Options::IsEmitIRBeforeAction() const {
//   return primaryAction == PrimaryAction::EmitIRBefore;
// }

// bool Options::IsEmitModuleAction() const {
//   return primaryAction == PrimaryAction::EmitModule;
// }
// bool Options::IsEmitBCAction() const {
//   return primaryAction == PrimaryAction::EmitBC;
// }
// bool Options::IsEmitObjectAction() const {
//   return primaryAction == PrimaryAction::EmitObject;
// }
// bool Options::IsEmitAssemblyAction() const {
//   return primaryAction == PrimaryAction::EmitAssembly;
// }

// llvm::StringRef Options::GetActionString(PrimaryAction action) {
//   // TODO: It would be nice to pull this out of the OptTable -- atuo gen into
//   a
//   // file
//   switch (action) {
//   case PrimaryAction::None:
//     return "none";
//   case PrimaryAction::PrintVersion:
//     return "version";
//   case PrimaryAction::PrintHelp:
//     return "help";
//   case PrimaryAction::PrintHelpHidden:
//     return "help-hidden";
//   case PrimaryAction::PrintFeature:
//     return "feature";
//   case PrimaryAction::Parse:
//     return "parse";
//   case PrimaryAction::ResolveImports:
//     return "resolve-imports";
//   case PrimaryAction::PrintASTBefore:
//     return "print-ast-before";
//   case PrimaryAction::TypeCheck:
//     return "type-check";
//   case PrimaryAction::PrintASTAfter:
//     return "print-ast-after";
//   case PrimaryAction::EmitIRBefore:
//     return "emit-ir-before";
//   case PrimaryAction::EmitIRAfter:
//     return "emit-ir-after";
//   case PrimaryAction::PrintIR:
//     return "print-ir";
//   case PrimaryAction::EmitBC:
//     return "emit-bc";
//   case PrimaryAction::EmitModule:
//     return "emit-module";
//   case PrimaryAction::MergeModules:
//     return "merge-modules";
//   case PrimaryAction::EmitObject:
//     return "emit-object";
//   case PrimaryAction::EmitAssembly:
//     return "emit-assembly";
//   }
//   llvm_unreachable("Unhandled action");
// }
// // llvm::StringRef Options::GetActionString(const llvm::opt::Arg* arg) const
// {
// //   assert(arg);
// //   return arg->getOption().getName();
// // }
