#ifndef STONE_SYNTAX_SYNTAXSCOPE_H
#define STONE_SYNTAX_SYNTAXSCOPE_H

namespace stone {
namespace syn {

enum class SyntaxScopeKind {

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
  CompoundStmt,

  /// We are between inheritance colon and the real class/struct definition
  /// scope.
  Interface,

};

class SyntaxScope final {
public:
  enum Bits {
    /// This indicates that the scope corresponds to a function, which
    /// means that labels are set here.
    Fun = 0x01,

    /// This is a while, do, switch, for, etc that can have break
    /// statements embedded into it.
    Break = 0x02,

    /// This is a while, do, for, which can have continue statements
    /// embedded into it.
    Continue = 0x04,

    /// This is a scope that can contain a declaration.  Some scopes
    /// just contain loop constructs but don't contain decls.
    Decl = 0x08,

    /// The controlling scope in a if/switch/while/for statement.
    Control = 0x10,

    /// The scope of a struct/union/class definition.
    Struct = 0x20,

    /// This is a scope that corresponds to a block/closure object.
    /// Blocks serve as top-level scopes for some objects like labels, they
    /// also prevent things like break and continue.  BlockScopes always have
    /// the FunScope and DeclScope flags set as well.
    Block = 0x40,

    /// This is a scope that corresponds to the
    /// template parameters of a C++ template. Template parameter
    /// scope starts at the 'template' keyword and ends when the
    /// template declaration ends.
    TemplateParam = 0x80,

    /// This is a scope that corresponds to the
    /// parameters within a function prototype.
    FunctionPrototype = 0x100,

    /// This is a scope that corresponds to the parameters within
    /// a function prototype for a function declaration (as opposed to any
    /// other kind of function declarator). Always has FunctionPrototypeScope
    /// set as well.
    FunDecl = 0x200,

    /// This scope corresponds to an enum.
    Enum = 0x40000,

    /// This is a compound statement scope.
    CompoundStmt = 0x400000,

    /// We are between inheritance colon and the real class/struct definition
    /// scope.
    Interface = 0x800000,
  };
  SyntaxScope();
  ~SyntaxScope();
};
} // namespace syn
} // namespace stone
#endif
