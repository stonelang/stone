

/// Represents a nested name specifier, such as
/// "\::std::vector<int>::".
///
/// Nested name specifiers are the prefixes to qualified
/// names. For example, "foo::" in "foo::x" is a nested name
/// specifier. Nested name specifiers are made up of a sequence of
/// specifiers, each of which can be a namespace, type, identifier
/// (for dependent names), decltype specifier, or the global specifier ('::').
/// The last two specifiers can only appear at the start of a
/// nested-namespace-specifier.
class NestedNameSpecifier : public llvm::FoldingSetNode {
  /// Enumeration describing
  enum class StoredSpecifierKind : uint8_t {
    Identifier = 0,
    Decl = 1,
    TypeSpecifier = 2,
    TypeSpecWithTemplate = 3
  };
  /// The nested name specifier that precedes this nested name
  /// specifier.
  ///
  /// The pointer is the nested-name-specifier that precedes this
  /// one. The integer stores one of the first four values of type
  /// SpecifierKind.
  llvm::PointerIntPair<NestedNameSpecifier *, 2, StoredSpecifierKind> prefix;

public:
  /// The kind of specifier that completes this nested name
  /// specifier.
  enum class SpecifierKind {
    /// An identifier, stored as an IdentifierInfo*.
    Identifier,

    /// A namespace, stored as a NamespaceDecl*.
    Namespace,

    /// A namespace alias, stored as a NamespaceAliasDecl*.
    NamespaceAlias,

    /// A type, stored as a Type*.
    TypeSpecifier,

    /// A type that was preceded by the 'template' keyword,
    /// stored as a Type*.
    TypeSpecifierWithTemplate,

    /// The global specifier '::'. There is no stored value.
    Global,
  };
};