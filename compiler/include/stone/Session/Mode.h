#ifndef STONE_SESSION_MODE_H
#define STONE_SESSION_MODE_H

#include "stone/Basic/File.h"
#include "stone/Session/ModeKind.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class Mode final {
  friend class Session;
  ModeKind kind;
  llvm::StringRef name;

public:
  Mode(ModeKind kind = ModeKind::None, llvm::StringRef name = llvm::StringRef())
      : kind(kind), name(name) {}

public:
  ModeKind GetKind() const { return kind; }
  llvm::StringRef GetName() const { return name; }
  void SetName(llvm::StringRef v) { name = v; }
  file::Type GetOutputFileType() const;

  bool CanOutput() const {
    switch (GetKind()) {
    case ModeKind::DumpSyntax:
    case ModeKind::PrintSyntax:
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
    case ModeKind::ResolveImports:
    case ModeKind::DumpSyntax:
    case ModeKind::TypeCheck:
    case ModeKind::PrintSyntax:
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

  bool CanCodeGen() const {
    switch (GetKind()) {
    case ModeKind::None:
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
  bool IsSupport() {
    switch (GetKind()) {
    case ModeKind::PrintHelp:
    case ModeKind::PrintVersion:
    case ModeKind::PrintFeature:
      return true; 
    default:
      return false;
    }
  }

  bool JustParse() {
    switch (GetKind()) {
    case ModeKind::Parse:
    case ModeKind::DumpSyntax:
      return true;
      break;
      return false;
    }
  }
  bool JustTypeCheck() {
    switch (GetKind()) {
    case ModeKind::TypeCheck:
    case ModeKind::PrintSyntax:
      return true;
      break;
      return false;
    }
  }

public:
  // Convenience
  bool IsNone() const { return GetKind() == ModeKind::None; }
  bool IsPrintHelp() { return GetKind() == ModeKind::PrintHelp; }
  bool IsPrintVersion() { return GetKind() == ModeKind::PrintVersion; }
  bool IsPrintFeature() { return GetKind() == ModeKind::PrintFeature; }
  bool IsParse() { return GetKind() == ModeKind::Parse; }
  bool IsDumpSyntax() { return GetKind() == ModeKind::DumpSyntax; }
  bool IsTypeCheck() { return GetKind() == ModeKind::TypeCheck; }
  bool IsPrintSyntax() { return GetKind() == ModeKind::PrintSyntax; }
  bool IsEmitIR() { return GetKind() == ModeKind::EmitIR; }
  bool IsEmitModule() { return GetKind() == ModeKind::EmitModule; }
  bool IsEmitLibrary() { return GetKind() == ModeKind::EmitLibrary; }
  bool IsEmitBC() { return GetKind() == ModeKind::EmitBC; }
  bool IsEmitObject() { return GetKind() == ModeKind::EmitObject; }
  bool IsEmitAssembly() { return GetKind() == ModeKind::EmitAssembly; }
  bool IsAlien() { return GetKind() == ModeKind::Alien; }

public:
  bool Is(ModeKind k) const { return kind == k; }
  bool IsAny(ModeKind K1) const { return Is(K1); }
  template <typename... T> bool IsAny(ModeKind K1, ModeKind K2, T... K) const {
    if (Is(K1)) {
      return true;
    }
    return IsAny(K2, K...);
  }
  template <typename... T> bool IsNot(ModeKind K1, T... K) const {
    return !IsAny(K1, K...);
  }

public:
  static file::Type GetOutputFileTypeByModeKind(ModeKind kind);
  static std::unique_ptr<Mode> Create(const llvm::opt::InputArgList &ial);
};
} // namespace stone
#endif
