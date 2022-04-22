#ifndef STONE_SESSION_MODE_H
#define STONE_SESSION_MODE_H

#include "stone/Basic/File.h"
#include "stone/Session/ModeKind.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class Mode final {
  friend class Session;
  ModeKind kind;

private:
  llvm::StringRef name;
  void SetName(llvm::StringRef v) { name = v; }

public:
  Mode(ModeKind kind, llvm::StringRef name = llvm::StringRef())
      : kind(kind), name(name) {}

public:
  ModeKind GetKind() const { return kind; }
  llvm::StringRef GetName() const { return name; }
  bool Is(ModeKind k) const { return kind == k; }

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
    case ModeKind::ResolveUsings:
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
  bool IsNone() const { return GetKind() == ModeKind::None; }
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
  file::Type GetOutputFileType();

public:
  static file::Type GetOutputFileType(ModeKind kind);
  static std::unique_ptr<Mode> Create(const llvm::opt::InputArgList &ial);
};

} // namespace stone
#endif
