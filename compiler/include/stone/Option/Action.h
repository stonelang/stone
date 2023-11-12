#ifndef STONE_OPTION_ACTION_H
#define STONE_OPTION_ACTION_H

#include "stone/Basic/File.h"
#include "llvm/Option/ArgList.h"

namespace stone {

// The modes that the system supports
enum class ActionKind : unsigned {
  ///< No mode
  None = 0,
  ///< Parse only
  Parse,
  ///< Parse and resolve use(s) only
  ResolveImports,
  ///< Parse and dump syntax tree
  DumpSyntax,
  ///< Parse and type-check only
  TypeCheck,
  ///< TODO
  DumpTypeInfo,
  ///< Parse, type-check, and  pretty print syntax tree
  PrintSyntax,
  //</ Parse, type-check, and pretty print llvm-ir
  PrintIR,
  //</ Parse, type-check, and emit LLVM IR pre optimization
  EmitIRBefore,
  //</ Parse, type-check, and emit LLVM IR post optimization
  EmitIRAfter,
  //< Parse, type-check, and emit LLVM BC
  EmitBC,
  ///< Parse, type-check, and emit native object code
  EmitObject,
  ///< Parse, type-check, and emit a library.
  ///< Default => platform specific. But, with -static => 'any.a'
  EmitLibrary,
  ///< Create a module fule
  InitModule,
  //< Parse, type-check, and emit a module. Ex: 'any.stonemod'
  EmitModule,
  //< Parse, type-check, and emit assembly
  EmitAssembly,
  //< Print language version
  PrintVersion,
  ///< Print help
  PrintHelp,
  ///< Print help
  PrintHelpHidden,
  ///< Merge all modules
  MergeModules,
  //< Alien
  Alien,
};

class Action {
  ActionKind kind;

protected:
  llvm::StringRef name;
  void SetName(llvm::StringRef v) { name = v; }

public:
  Action();

public:
  ActionKind GetKind() const { return kind; }
  llvm::StringRef GetName() const { return name; }
  bool Is(ActionKind k) const { return kind == k; }
  file::Type GetOutputFileType() const;

  bool CanOutput() const {
    switch (GetKind()) {
    case ActionKind::DumpSyntax:
    case ActionKind::PrintSyntax:
    case ActionKind::EmitIRBefore:
    case ActionKind::EmitIRAfter:
    case ActionKind::EmitBC:
    case ActionKind::EmitObject:
    case ActionKind::EmitAssembly:
    case ActionKind::EmitModule:
    case ActionKind::EmitLibrary:
      return true;
    default:
      return false;
    }
  }
  bool CanCompile() const {
    switch (GetKind()) {
    case ActionKind::None:
    case ActionKind::Parse:
    case ActionKind::ResolveImports:
    case ActionKind::DumpSyntax:
    case ActionKind::TypeCheck:
    case ActionKind::PrintSyntax:
    case ActionKind::EmitIRBefore:
    case ActionKind::EmitIRAfter:
    case ActionKind::EmitBC:
    case ActionKind::EmitObject:
    case ActionKind::EmitAssembly:
    case ActionKind::EmitModule:
    case ActionKind::EmitLibrary:
      return true;
    default:
      return false;
    }
  }

  bool CanCodeGen() const {
    switch (GetKind()) {
    case ActionKind::None:
    case ActionKind::EmitIRAfter:
    case ActionKind::EmitIRBefore:
    case ActionKind::EmitBC:
    case ActionKind::EmitObject:
    case ActionKind::EmitAssembly:
    case ActionKind::EmitModule:
    case ActionKind::EmitLibrary:
      return true;
    default:
      return false;
    }
  }
  bool IsImmediate() {
    switch (GetKind()) {
    case ActionKind::PrintHelp:
    case ActionKind::PrintVersion:
      return true;
    default:
      return false;
    }
  }
  bool IsValid() {
    if (CanCompile()) {
      return true;
    }
    if (IsImmediate()) {
      return true;
    }
    return false;
  }

public:
  // Convenience
  bool IsNone() const { return GetKind() == ActionKind::None; }
  bool IsPrintHelp() const { return GetKind() == ActionKind::PrintHelp; }
  bool IsPrintHelpHidden() const {
    return GetKind() == ActionKind::PrintHelpHidden;
  }
  bool IsPrintVersion() const { return GetKind() == ActionKind::PrintVersion; }
  bool IsParse() const { return GetKind() == ActionKind::Parse; }
  bool IsDumpSyntax() const { return GetKind() == ActionKind::DumpSyntax; }
  bool IsTypeCheck() const { return GetKind() == ActionKind::TypeCheck; }
  bool IsDumpTypeInfo() const { return GetKind() == ActionKind::DumpTypeInfo; }
  bool IsPrintSyntax() const { return GetKind() == ActionKind::PrintSyntax; }
  bool IsEmitIRAfter() const { return GetKind() == ActionKind::EmitIRAfter; }
  bool IsEmitIRBefore() const { return GetKind() == ActionKind::EmitIRBefore; }
  bool IsEmitIR() const { return (IsEmitIRAfter() || IsEmitIRBefore()); }
  bool IsEmitModule() const { return GetKind() == ActionKind::EmitModule; }
  bool IsEmitLibrary() const { return GetKind() == ActionKind::EmitLibrary; }
  bool IsEmitBC() { return GetKind() == ActionKind::EmitBC; }
  bool IsEmitObject() const { return GetKind() == ActionKind::EmitObject; }
  bool IsEmitAssembly() const { return GetKind() == ActionKind::EmitAssembly; }
  bool IsAlien() const { return GetKind() == ActionKind::Alien; }

public:
  static file::Type GetOutputFileTypeByActionKind(ActionKind kind);
};
} // namespace stone
#endif
