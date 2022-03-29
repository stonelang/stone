#ifndef STONE_SYNTAX_SYNTAXSCOPE_H
#define STONE_SYNTAX_SYNTAXSCOPE_H

#include <stdint.h>

namespace stone {
namespace syn {

enum class SyntaxScopeKind : uint8_t {

  /// This indicates that the scope corresponds to a function, which
  /// means that labels are set here.
  Fun,

  /// This is a while, do, switch, for, etc that can have break
  /// statements embedded into it.
  Break,

  /// This is a while, do, for, which can have continue statements
  /// embedded into it.
  Continue,

  /// This is a scope that can contain a declaration.  Some scopes
  /// just contain loop constructs but don't contain decls.
  Decl,

  /// The controlling scope in a if/switch/while/for statement.
  Control,

  /// The scope of a struct/union/class definition.
  Struct,

  /// We are between inheritance colon and the real class/struct definition
  /// scope.
  Interface,

  /// This is a scope that corresponds to a block/closure object.
  /// Blocks serve as top-level scopes for some objects like labels, they
  /// also prevent things like break and continue.  BlockScopes always have
  /// the FunScope and DeclScope flags set as well.
  Block,

  /// This is a scope that corresponds to the
  /// template parameters of a C++ template. Template parameter
  /// scope starts at the 'template' keyword and ends when the
  /// template declaration ends.
  TemplateParam,

  /// This is a scope that corresponds to the
  /// parameters within a function prototype.
  FunctionSignature,

  /// This is a scope that corresponds to the parameters within
  /// a function prototype for a function declaration (as opposed to any
  /// other kind of function declarator). Always has FunctionPrototypeScope
  /// set as well.
  FunDecl,

  /// This scope corresponds to an enum.
  Enum,

  /// This is a compound statement scope.
  BlockStmt,

};

class SyntaxScope final {
  SyntaxScope *parent = nullptr;
  SyntaxScopeKind kind;

public:
  SyntaxScope(SyntaxScopeKind kind, SyntaxScope *parent = nullptr);
  ~SyntaxScope();
};
} // namespace syn
} // namespace stone
#endif
