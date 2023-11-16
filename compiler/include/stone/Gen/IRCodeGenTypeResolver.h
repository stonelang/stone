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

class IRCodeGenModule;

class IRCodeGenTypeResolver final {
  IRCodeGenModule &cgm;

public:
  IRCodeGenTypeResolver(IRCodeGenModule &cgm);

public:
  llvm::Type *GetType(const Type ty);

  // llvm::Type *GetType(const MemberPointerType *mpt);
  llvm::FunctionType *GetFunctionType(const FunctionDecl *fd);
};

} // namespace stone
#endif
