#ifndef STONE_GEN_IRCODEGENTYPERESOLVER_H
#define STONE_GEN_IRCODEGENTYPERESOLVER_H

namespace llvm {
class Type;
class FunctionType;
class IntegerType;
class PointerType;
} // namespace llvm

namespace stone {

class Type;
class Decl;
class FunctionDecl;
class MemberPointerType;

class CodeGenModule;
class CodeGenTypeResolver final {
  CodeGenModule &cgm;

public:
  CodeGenTypeResolver(CodeGenModule &cgm);

public:
  llvm::Type *GetType(const stone::Type ty);

  // llvm::Type *GetType(const stone::MemberPointerType *mpt);
  llvm::FunctionType *GetFunctionType(const stone::FunctionDecl *fd);
};

} // namespace stone
#endif
