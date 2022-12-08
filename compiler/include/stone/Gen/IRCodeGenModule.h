#ifndef STONE_GEN_IRCODEGENMODULE_H
#define STONE_GEN_IRCODEGENMODULE_H

#include "stone/Syntax/Module.h"

#include "llvm/IR/Module.h"

#include <memory>

using namespace stone::syn;

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class IRCodeGen;

// I do not think that you need this -- just use IRCodeGen
class IRCodeGenModule final {
  IRCodeGen &irCodeGen;

private:
  IRCodeGenModule(const IRCodeGenModule &) = delete;
  void operator=(const IRCodeGenModule &) = delete;

public:
  IRCodeGenModule(IRCodeGen &irCodeGen);
  ~IRCodeGenModule();

public:
  void EmitSyntaxFile(const SyntaxFile &sf);
  void EmitDecl(Decl *d);
  void EmitFunDecl(FunDecl *d);
  void EmitInterfaceDecl(InterfaceDecl *d);
  void EmitStructDecl(StructDecl *d);
  void EmitEnumDecl(EnumDecl *d);
};
} // namespace stone

#endif
