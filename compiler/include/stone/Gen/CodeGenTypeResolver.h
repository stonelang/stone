#ifndef STONE_GEN_IRCODEGENTYPERESOLVER_H
#define STONE_GEN_IRCODEGENTYPERESOLVER_H

namespace llvm {
class Type;
class FunctionType;
class IntegerType;
class PointerType;
} // namespace llvm

namespace stone {
namespace syn {
class Type;
class Decl;
class FunctionDecl;
class MemberPointerType;
} // namespace syn

class CodeGenModule;

class CodeGenTypeResolver final {
  CodeGenModule &cgm;

public:
  CodeGenTypeResolver(CodeGenModule &cgm);

public:
  llvm::Type *GetType(const syn::Type ty);

  // llvm::Type *GetType(const syn::MemberPointerType *mpt);
  llvm::FunctionType *GetFunctionType(const syn::FunctionDecl *fd);
};

} // namespace stone
#endif
