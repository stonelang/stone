#ifndef STONE_GEN_IRCODEGENTYPERESOLVER_H
#define STONE_GEN_IRCODEGENTYPERESOLVER_H

namespace llvm {
class Type;
class IntegerType;
class PointerType;
} // namespace llvm

namespace stone {
namespace syn {
class Type;
}

class IRCodeGenModule;

class IRCodeGenTypeResolver final {
  IRCodeGenModule &cgm;

public:
  IRCodeGenTypeResolver(IRCodeGenModule &cgm);

public:
  llvm::Type *ResolveType(syn::Type ty);
};

} // namespace stone
#endif
