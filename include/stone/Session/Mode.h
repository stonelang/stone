#ifndef STONE_SESSION_MODE_H
#define STONE_SESSION_MODE_H

#include "stone/Session/ModeType.h"

namespace stone {

class Mode final {
  friend class Session;
  ModeType ty;
  llvm::StringRef name;

  void SetType(ModeType k) { ty = k; }
  void SetName(llvm::StringRef v) { name = v; }

private:
  bool IsCompileOnlyImpl() const {
    switch (GetType()) {
    case ModeType::Parse:
    case ModeType::TypeCheck:
    case ModeType::EmitIR:
    case ModeType::EmitBC:
    case ModeType::EmitObject:
    case ModeType::EmitModule:
    case ModeType::EmitLibrary:
    case ModeType::EmitAssembly:
      return true;
    default:
      return false;
    }
  }

public:
  ModeType GetType() const { return ty; }
  llvm::StringRef GetName() const { return name; }

  bool Is(ModeType mt) const { return ty == mt; }

  bool CanOutput() {
    switch (GetType()) {
    case ModeType::EmitIR:
    case ModeType::EmitBC:
    case ModeType::EmitObject:
    case ModeType::EmitAssembly:
    case ModeType::EmitModule:
    case ModeType::EmitLibrary:
    case ModeType::EmitExecutable:
      return true;
    default:
      return false;
    }
  }
  bool IsCompileOnly() const {
    if (IsLinkable())
      return false;
    return IsCompileOnlyImpl();
  }

  bool IsCompilable() const {
    switch (GetType()) {
    case ModeType::EmitExecutable:
      return true;
    default:
      return IsCompileOnlyImpl();
    }
  }
  bool IsLinkOnly() const { return GetType() == ModeType::Link; }
  bool IsLinkable() const {
    switch (GetType()) {
    case ModeType::EmitExecutable:
    case ModeType::Link:
      return true;
    default:
      return false;
    }
  }
  bool IsValid() {
    switch (GetType()) {
    case ModeType::Parse:
    case ModeType::TypeCheck:
    case ModeType::EmitIR:
    case ModeType::EmitBC:
    case ModeType::EmitObject:
    case ModeType::EmitAssembly:
    case ModeType::EmitModule:
    case ModeType::EmitLibrary:
    case ModeType::EmitExecutable:
      return true;
    default:
      return false;
    }
  }
  static llvm::StringRef GetNameByType(ModeType ty);

private:
  Mode(ModeType ty) : ty(ty) {}
};
} // namespace stone
#endif
