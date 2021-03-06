#ifndef STONE_CODEGEN_CODEEMITTER_H
#define STONE_CODEGEN_CODEEMITTER_H

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {
namespace syn {
class Decl;
}
namespace codegen {

class CodeEmitter final {
public:
  CodeEmitter();

public:
  void EmitStmt();

public:
  void EmitDecl(const syn::Decl &decl);

public:
  void EmitExpr();
};
} // namespace codegen
} // namespace stone
#endif
