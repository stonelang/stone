#ifndef STONE_OPTIONS_ACTION_H
#define STONE_OPITONS_ACTION_H

namespace stone {
// The modes that the system supports
enum class ActionKind : unsigned {
  ///< No mode
  None = 0,
  ///< Parse only
  Parse,
  ///< Parse and resolve use(s) only
  ResolveImports,
  ///< Parse and dump asttax tree
  DumpAST,
  ///< Parse and type-check only
  TypeCheck,
  ///< Parse, type-check, and  pretty print asttax tree
  PrintAST,
  ///< Parse, type-check, and  dump type-into ( --dump-typeinfo)
  DumpTypeInfo,
  //</ Parse, type-check, and pretty print llvm-ir
  PrintIR,
  //</ Parse, type-check, and emit LLVM IR pre optimization
  EmitIRPre,
  //</ Parse, type-check, and emit LLVM IR post optimization
  EmitIR,
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
  ///< Merge all modules
  MergeModules,
  //< Alien
  Alien,
};

class Action {
  ActionKind kind;
  llvm::StringRef name;

protected:
  void SetName(llvm::StringRef v) { name = v; }
  void SetKind(ActionKind inputKind) { kind = inputKind; }

public:
  Action() {}
  ~Action() {}

public:
  ActionKind GetKind() const { return kind; }
  llvm::StringRef GetName() const { return name; }
  bool IsAction(ActionKind k) const { return kind == k; }

public:
  bool CanCompile() const {
  case ActionKind::None:
  case ActionKind::Parse:
  case ActionKind::ResolveImports:
  case ActionKind::DumpAST:
  case ActionKind::TypeCheck:
  case ActionKind::PrintAST:
  case ActionKind::EmitIR:
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


  bool CanOutput() const {
  switch (GetKind()) {
  case ActionKind::DumpAST:
  case ActionKind::PrintAST:
  case ActionKind::EmitIR:
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
  case ActionKind::EmitIR:
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
bool IsImmediateOnly() {
  switch (GetKind()) {
  case ActionKind::PrintHelp:
  case ActionKind::PrintVersion:
    return true;
  default:
    return false;
  }
}

bool IsParseOnly() {
  switch (GetKind()) {
  case ActionKind::Parse:
  case ActionKind::DumpAST:
    return true;
    break;
    return false;
  }
}
bool IsTypeCheckOnly() {
  switch (GetKind()) {
  case ActionKind::TypeCheck:
  case ActionKind::PrintAST:
    return true;
    break;
    return false;
  }
}

public:
// Convenience
bool IsNone() const { return GetKind() == ActionKind::None; }
bool IsPrintHelp() { return GetKind() == ActionKind::PrintHelp; }
bool IsPrintVersion() { return GetKind() == ActionKind::PrintVersion; }
bool IsParse() { return GetKind() == ActionKind::Parse; }
bool IsResolveImports() { return GetKind() == ActionKind::ResolveImports; }
bool IsDumpAST() { return GetKind() == ActionKind::DumpAST; }
bool IsTypeCheck() { return GetKind() == ActionKind::TypeCheck; }
bool IsDumpTypeInfo() { return GetKind() == ActionKind::DumpTypeInfo; }
bool IsPrintAST() { return GetKind() == ActionKind::PrintAST; }
bool IsPrintIR() { return GetKind() == ActionKind::PrintIR; }
bool IsEmitIR() { return GetKind() == ActionKind::EmitIR; }
bool IsEmitModule() { return GetKind() == ActionKind::EmitModule; }
bool IsEmitLibrary() { return GetKind() == ActionKind::EmitLibrary; }
bool IsEmitBC() { return GetKind() == ActionKind::EmitBC; }
bool IsEmitObject() { return GetKind() == ActionKind::EmitObject; }
bool IsEmitAssembly() { return GetKind() == ActionKind::EmitAssembly; }
bool IsAlien() { return GetKind() == ActionKind::Alien; }

};
} // namespace stone
#endif
