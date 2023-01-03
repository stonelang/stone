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
} // namespace syn

class IRCodeGenModule;

class IRCodeGenTypeResolver final {
  IRCodeGenModule &cgm;

public:
  IRCodeGenTypeResolver(IRCodeGenModule &cgm);

public:
  llvm::Type *ResolveType(syn::Type ty);
  llvm::FunctionType *ResolveFunctionType(syn::Decl &fd);
};

} // namespace stone
#endif
