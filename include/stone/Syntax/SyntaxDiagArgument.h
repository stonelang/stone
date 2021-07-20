#ifndef STONE_SYNTAX_SYNTAXDIAGARGUMENT_H
#define STONE_SYNTAX_SYNTAXDIAGARGUMENT_H

#include "stone/Basic/DiagnosticArgument.h"
#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Identifier.h"

using stone::Diagnostic;

#include <assert.h>

namespace stone {

class DiagnosticEngine;

namespace diag {
enum class SyntaxArgumentType { Decl, DeclContext, Type, Identifier };

// TODO: Hmmm...I mal only need SyntaxArg(SyntaxArgType ty, const void* val)
//  Example call: diag::SyntaxArg(SyntaxArgType::Decl, theDecl);
struct SyntaxArgument : public ComplexArgument {
  SyntaxArgumentType synTy;

public:
  SyntaxArgument() = delete;
  explicit SyntaxArgument(SyntaxArgumentType synTy, const void *val)
      : ComplexArgument(val), synTy(synTy) {}
  SyntaxArgumentType GetSyntaxArgumentType() { return synTy; }
};

struct DeclArgument final : public SyntaxArgument {

  DeclArgument() = delete;
  explicit DeclArgument(const syn::Decl *val)
      : SyntaxArgument(SyntaxArgumentType::Decl, val) {}
};

struct DeclContextArgument final : public SyntaxArgument {

  DeclContextArgument() = delete;
  explicit DeclContextArgument(const syn::DeclContext *val)
      : SyntaxArgument(SyntaxArgumentType::DeclContext, val) {}
};

struct IdentifierArgument final : public SyntaxArgument {

  IdentifierArgument() = delete;
  explicit IdentifierArgument(const syn::Identifier *val)
      : SyntaxArgument(SyntaxArgumentType::Identifier, val) {}
};

struct TypeArgument final : public SyntaxArgument {
  TypeArgument() = delete;
  explicit TypeArgument(const syn::Type *val)
      : SyntaxArgument(SyntaxArgumentType::Type, val) {}
};

} // namespace diag
class SyntaxDiagnostic final : public Diagnostic {
public:
  explicit SyntaxDiagnostic(DiagnosticContext context) : Diagnostic(context) {}

public:
  void Format(llvm::SmallVectorImpl<char> &outStr,
              const DiagnosticFormatOptions &fmtOptions) const override;

  void Format(const char *diagStr, const char *diagEnd,
              llvm::SmallVectorImpl<char> &outStr,
              const DiagnosticFormatOptions &fmtOptions) const override;
};

} // namespace stone

#endif
