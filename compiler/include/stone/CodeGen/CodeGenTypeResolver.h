#ifndef STONE_GEN_IRCODEGENTYPERESOLVER_H
#define STONE_GEN_IRCODEGENTYPERESOLVER_H

namespace llvm {
class Type;
class FunctionType;
class IntegerType;
class PointerType;
} // namespace llvm

namespace stone {
namespace ast {
class Type;
class Decl;
class FunctionDecl;
class MemberPointerType;
} // namespace ast

namespace codegen {
class CodeGenModule;
class CodeGenTypeResolver final {
  CodeGenModule &cgm;

public:
  CodeGenTypeResolver(CodeGenModule &cgm);

public:
  llvm::Type *GetType(const ast::Type ty);

  // llvm::Type *GetType(const ast::MemberPointerType *mpt);
  llvm::FunctionType *GetFunctionType(const ast::FunctionDecl *fd);
};
} // namespace codegen
} // namespace stone
#endif
