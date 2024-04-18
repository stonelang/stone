#ifndef STONE_OPTION_ACTION_H
#define STONE_OPTION_ACTION_H

#include "stone/Basic/FileType.h"
#include "stone/Option/ActionKind.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class Action final {
  ActionKind kind;
  llvm::StringRef name;

public:
  Action(ActionKind kind = ActionKind::None,
         llvm::StringRef name = llvm::StringRef())
      : kind(kind), name(name) {}

public:
  ActionKind GetKind() const { return kind; }
  llvm::StringRef GetName() const;
  file::FileType GetOutputFileType() const;

public:
  static llvm::StringRef GetName(ActionKind kind);
  static bool ShouldParseOnly(ActionKind kind);
  static bool ShouldGenerateOutput(ActionKind kind);
  static bool ShouldCompile(ActionKind kind);
  static bool ShouldGenerateCode(ActionKind kind);
  static bool ShouldGenerateIR(ActionKind kind);
  static bool ShouldGenerateNative(ActionKind kind);
  static file::FileType GetOutputFileTypeByActionKind(ActionKind kind);

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
  
  bool IsSupport() const {
    if (IsPrintHelp() || IsPrintHelpHidden() || IsPrintVersion()) {
      return true;
    }
    return false;
  }

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

  bool ShouldParseOnly() const { return Action::ShouldParseOnly(GetKind()); }
  bool ShouldGenerateOutput() const {
    return Action::ShouldGenerateOutput(GetKind());
  }
  bool ShouldCompile() const { return Action::ShouldCompile(GetKind()); }
  bool ShouldGenerateCode() const {
    return Action::ShouldGenerateCode(GetKind());
  }
  bool ShouldGenerateIR() const { return Action::ShouldGenerateIR(GetKind()); }
  bool ShouldGenerateNative() const {
    return Action::ShouldGenerateNative(GetKind());
  }

public:
  static Action Create(const llvm::opt::ArgList &args);
};

} // namespace stone
#endif
