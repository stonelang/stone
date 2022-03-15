#ifndef STONE_OPTION_MODE_H
#define STONE_OPTION_MODE_H

#include "stone/Option/ModeKind.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class Mode final {
  ModeKind kind;

public:
  Mode(ModeKind kind) : kind(kind) {}

public:
  ModeKind GetKind() const { return kind; }
  // llvm::StringRef GetName() const { return Mode::GetNameByKind }
  bool Is(ModeKind k) const { return kind == k; }

  bool CanOutput() {
    switch (GetKind()) {
    case ModeKind::EmitParse:
    case ModeKind::EmitSyntax:
    case ModeKind::EmitIR:
    case ModeKind::EmitBC:
    case ModeKind::EmitObject:
    case ModeKind::EmitAssembly:
    case ModeKind::EmitModule:
    case ModeKind::EmitLibrary:
      return true;
    default:
      return false;
    }
  }
  bool CanCompile() const {
    switch (GetKind()) {
    case ModeKind::None:
    case ModeKind::Parse:
    case ModeKind::EmitParse:
    case ModeKind::TypeCheck:
    case ModeKind::EmitSyntax:
    case ModeKind::EmitIR:
    case ModeKind::EmitBC:
    case ModeKind::EmitObject:
    case ModeKind::EmitAssembly:
    case ModeKind::EmitModule:
    case ModeKind::EmitLibrary:
      return true;
    default:
      return false;
    }
  }
  bool IsValid() {
    if (CanCompile()) {
      return true;
    }
    switch (GetKind()) {
    case ModeKind::PrintHelp:
    case ModeKind::PrintVersion:
      return true;
    default:
      return false;
    }
  }
  bool IsPrintHelp() { return GetKind() == ModeKind::PrintHelp; }
  bool IsPrintVersion() { return GetKind() == ModeKind::PrintVersion; }

public:
  // Convenience
  bool IsNone() { return GetKind() == ModeKind::None; }
  bool IsParse() { return GetKind() == ModeKind::Parse; }
  bool IsEmitParse() { return GetKind() == ModeKind::EmitParse; }
  bool IsTypeCheck() { return GetKind() == ModeKind::TypeCheck; }
  bool IsEmitSyntax() { return GetKind() == ModeKind::EmitSyntax; }
  bool IsEmitIR() { return GetKind() == ModeKind::EmitIR; }
  bool IsEmitBC() { return GetKind() == ModeKind::EmitBC; }
  bool IsEmitObject() { return GetKind() == ModeKind::EmitObject; }
  bool IsEmitAssembly() { return GetKind() == ModeKind::EmitAssembly; }
  bool IsEmitModule() { return GetKind() == ModeKind::EmitModule; }
  bool IsEmitLibrary() { return GetKind() == ModeKind::EmitLibrary; }
  bool IsAlien() { return GetKind() == ModeKind::Alien; }

  bool JustParse() {
    switch (GetKind()) {
    case ModeKind::Parse:
    case ModeKind::EmitParse:
      return true;
      break;
      return false;
    }
  }
  bool JustTypeCheck() {
    switch (GetKind()) {
    case ModeKind::TypeCheck:
    case ModeKind::EmitSyntax:
      return true;
      break;
      return false;
    }
  }

public:
  static llvm::StringRef GetNameByKind(ModeKind kind);
};

} // namespace stone
#endif
