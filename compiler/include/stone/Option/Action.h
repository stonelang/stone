#ifndef STONE_OPTION_ACTION_H
#define STONE_OPTION_ACTION_H

#include "stone/Basic/File.h"
#include "stone/Option/ActionKind.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class Action {
  ActionKind kind;

protected:
  llvm::StringRef name;
  void SetName(llvm::StringRef v) { name = v; }
  void SetKind(ActionKind inputKind) { kind = inputKind; }

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

  bool IsSupport() {
    if (IsPrintHelp() || IsPrintHelpHidden() || IsPrintVersion()) {
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
  bool IsResolveImports() const {
    return GetKind() == ActionKind::ResolveImports;
  }
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
