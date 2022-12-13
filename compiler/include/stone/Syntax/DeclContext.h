#ifndef STONE_SYNTAX_DECLCONTEXT_H
#define STONE_SYNTAX_DECLCONTEXT_H

#include <type_traits>

#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/DeclBits.h"
#include "stone/Syntax/DeclKind.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/Types.h"

#include "llvm/ADT/PointerEmbeddedInt.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
class raw_ostream;
}

namespace stone {
namespace syn {

class SyntaxContext;
class CanQualType;
class Decl;
class DeclContext;
class EnumDecl;
class ExtensionDecl;
class Expr;
class InterfaceDecl;
class SyntaxFile;
class Type;
class Module;
class NominalTypeDecl;
class ValueDecl;
class StructDecl;

enum class DeclContextKind : uint8_t {
  None,
  Module,
  ModuleFile,
  FunctionDecl,
};

class alignas(1 << DeclContextAlignInBits) DeclContext
    : public SyntaxAllocation<DeclContext> {

  enum class SyntaxHierarchy : unsigned {
    Decl,
    Expr,
    ModuleFile,
    Initializer,
    SerializedLocal,
    // If you add a new Tree hierarchies, then update the static_assert() below.
  };

  DeclContext *parent;
  llvm::PointerIntPair<DeclContext *, 3, SyntaxHierarchy> parentAndKind;

  static SyntaxHierarchy GetSyntaxHierarchyFromKind(DeclContextKind Kind) {
    switch (Kind) {
    case DeclContextKind::ModuleFile:
      return SyntaxHierarchy::ModuleFile;
    case DeclContextKind::Module:
    case DeclContextKind::FunctionDecl:
      return SyntaxHierarchy::Decl;
    }
    llvm_unreachable("Unhandled DeclContextKind");
  }

protected:
  /// This anonymous union stores the bits belonging to DeclContext and classes
  /// deriving from it. The goal is to use otherwise wasted
  /// space in DeclContext to store data belonging to derived classes.
  /// The space saved is especially significient when pointers are aligned
  /// to 8 bytes. In this case due to alignment requirements we have a
  /// little less than 8 bytes free in DeclContext which we can use.
  /// We check that none of the classes in this union is larger than
  /// 8 bytes with static_asserts in the ctor of DeclContext.
  union {
    DeclContextBits declContextBits;
    NominalTypeDeclBits nominalTypeDeclBits;
    // EnumDeclBitfields EnumDeclBits;
    // RecordDeclBitfields RecordDeclBits;
    FunctionDeclBits functionDeclBits;
    // ConstructorDeclBits constructorDeclBits;
    // LinkageSpecDeclBitfields LinkageSpecDeclBits;
    // BlockDeclBitfields BlockDeclBits;

    ModuleDeclBits moduleDeclBits;

    static_assert(sizeof(DeclContextBits) <= 8,
                  "DeclContextBitfields is larger than 8 bytes!");

    static_assert(sizeof(NominalTypeDeclBits) <= 8,
                  "TagDeclBitfields is larger than 8 bytes!");

    // static_assert(sizeof(EnumDeclBits) <= 8,
    //              "EnumDeclBitfields is larger than 8 bytes!");
    // static_assert(sizeof(RecordDeclBits) <= 8,
    //              "RecordDeclBitfields is larger than 8 bytes!");
    static_assert(sizeof(FunctionDeclBits) <= 8,
                  "FunctionDeclBitfields is larger than 8 bytes!");

    static_assert(sizeof(ModuleDeclBits) <= 8,
                  "ModuleDeclBitfields is larger than 8 bytes!");

    // static_assert(sizeof(ConstructorDeclBits) <= 8,
    //              "ConstructorDeclBitfields is larger than 8 bytes!");
    // static_assert(sizeof(LinkageSpecDeclBits) <= 8,
    //              "LinkageSpecDeclBitfields is larger than 8 bytes!");
    // static_assert(sizeof(BlockDeclBitfields) <= 8,
    //              "BlockDeclBitfields is larger than 8 bytes!");
  };

protected:
  /// FirstDecl - The first declaration stored within this declaration
  /// context.
  mutable Decl *firstDecl = nullptr;

  /// LastDecl - The last declaration stored within this declaration
  /// context. FIXME: We could probably cache this value somewhere
  /// outside of the DeclContext, to reduce the size of DeclContext by
  /// another pointer.
  mutable Decl *lastDecl = nullptr;

  /// Build up a chain of declarations.
  ///
  /// \returns the first/last pair of declarations.
  static std::pair<Decl *, Decl *> BuildDeclChain(llvm::ArrayRef<Decl *> decls,
                                                  bool fieldsAlreadyLoaded);

public:
  DeclContext(DeclContextKind kind, DeclContext *parent = nullptr);

public:
  DeclContextKind GetDeclContextKind() const;
  DeclContext *GetParent() { return parent; }

  Decl *CastToDecl() {
    return parentAndKind.getInt() == SyntaxHierarchy::Decl
               ? reinterpret_cast<Decl *>(this + 1)
               : nullptr;
  }
  const Decl *CastToDecl() const {
    return const_cast<DeclContext *>(this)->CastToDecl();
  }

  // Return the SyntaxContext for a specified DeclContext by
  /// walking up to the enclosing module and returning its SyntaxContext.
  SyntaxContext &GetSyntaxContext() const;

  /// Returns the semantic parent of this context.  A context has a
  /// parent if and only if it is not a module context.
  DeclContext *GetParent() const { parentAndKind.getPointer(); }
  bool IsModuleContext() const;

  ModuleDecl *GetParentModule() const;

  SyntaxFile *GetParentSyntaxFile() const;

  bool IsTypeContext() const;
};

} // namespace syn
} // namespace stone
#endif
