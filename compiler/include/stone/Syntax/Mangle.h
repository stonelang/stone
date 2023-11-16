#ifndef STONE_SYNTAX_MANGLE_H
#define STONE_SYNTAX_MANGLE_H

#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Global.h"
#include "stone/Syntax/LangABI.h"
#include "stone/Syntax/Type.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Casting.h"

namespace llvm {
class raw_ostream;
}

namespace stone {

class ASTContext;
class ConstructorDecl;
class DestructorDecl;
class FunctionDecl;
class NameableDecl;
class StringLiteral;
class VarDecl;

enum class MangleContextKind : UInt8 { None = 0, Itanium, Microsoft };

class MangleContext {
  ASTContext &sc;
  const MangleContextKind kind;

public:
  MangleContext(MangleContextKind kind, ASTContext &sc) : kind(kind), sc(sc) {}

public:
  /// Generates a unique string for an externally visible type for use with TBAA
  /// or type uniquing.
  /// TODO: Extend this to internal types by generating names that are unique
  /// across translation units so it can be used with LTO.
  // virtual void MangleTypeName(Type T, llvm::raw_ostream & os) = 0;

public:
  ASTContext &GetASTContext() { return sc; }
  MangleContextKind GetKind() { return kind; }
};

class ItaniumMangleContext : public MangleContext {

public:
  ItaniumMangleContext(ASTContext &sc)
      : MangleContext(MangleContextKind::Itanium, sc) {}
};

class MicrosoftMangleContext : public MangleContext {

public:
  MicrosoftMangleContext(ASTContext &sc)
      : MangleContext(MangleContextKind::Microsoft, sc) {}
};

} // namespace stone

#endif
