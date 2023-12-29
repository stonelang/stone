#ifndef STONE_OPTION_ACTION_H
#define STONE_OPTION_ACTION_H

#include "stone/Basic/File.h"
#include "stone/Option/ActionKind.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class Action final {
  ActionKind kind;
  llvm::StringRef name;

public:
  Action(ActionKind inputKind = ActionKind::Alien,
         llvm::StringRef inputName = llvm::StringRef())
      : kind(inputKind), name(inputName) {}

public:
  ActionKind GetKind() const { return kind; }
  llvm::StringRef GetName() const { return name; }
  file::Type GetOutputFileType() const;

public:
  bool CanOutput() const {
    switch (GetKind()) {
    case ActionKind::DumpAST:
    case ActionKind::PrintAST:
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
    case ActionKind::DumpAST:
    case ActionKind::TypeCheck:
    case ActionKind::PrintAST:
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
  bool IsPrintFeature() const { return GetKind() == ActionKind::PrintFeature; }
  bool IsParse() const { return GetKind() == ActionKind::Parse; }
  bool IsResolveImports() const {
    return GetKind() == ActionKind::ResolveImports;
  }
  bool IsDumpAST() const { return GetKind() == ActionKind::DumpAST; }
  bool IsTypeCheck() const { return GetKind() == ActionKind::TypeCheck; }
  bool IsPrintAST() const { return GetKind() == ActionKind::PrintAST; }
  bool IsEmitIRAfter() const { return GetKind() == ActionKind::EmitIRAfter; }
  bool IsEmitIRBefore() const { return GetKind() == ActionKind::EmitIRBefore; }
  bool IsEmitIR() const { return (IsEmitIRAfter() || IsEmitIRBefore()); }
  bool IsEmitModule() const { return GetKind() == ActionKind::EmitModule; }
  bool IsEmitLibrary() const { return GetKind() == ActionKind::EmitLibrary; }
  bool IsEmitBC() { return GetKind() == ActionKind::EmitBC; }
  bool IsEmitObject() const { return GetKind() == ActionKind::EmitObject; }
  bool IsEmitAssembly() const { return GetKind() == ActionKind::EmitAssembly; }
  bool IsAlien() const { return GetKind() == ActionKind::Alien; }

  bool Is(ActionKind k) const { return kind == k; }
  bool IsAny(ActionKind K1) const { return Is(K1); }
  template <typename... T>
  bool IsAny(ActionKind K1, ActionKind K2, T... K) const {
    if (Is(K1)) {
      return true;
    }
    return IsAny(K2, K...);
  }
  template <typename... T> bool IsNot(ActionKind K1, T... K) const {
    return !IsAny(K1, K...);
  }

public:
  static file::Type GetOutputFileTypeByActionKind(ActionKind kind);
};

namespace opts {
Action ParseAction(const llvm::opt::ArgList &args);
}

} // namespace stone
#endif
