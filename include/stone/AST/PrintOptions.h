#ifndef STONE_AST_PRINTOPTIONS_H
#define STONE_AST_PRINTOPTIONS_H

#include "stone/AST/Visibility.h"
#include "stone/Basic/Basic.h"

#include "llvm/ADT/PointerUnion.h"

namespace stone {

class PrintOptions final {
public:
  /// The indentation width.
  unsigned Indent = 2;

  /// Whether to print function definitions.
  bool FunctionDefinitions = false;

  /// Whether to print expressions.
  bool PrintExprs = false;

  /// Whether to print '{ get set }' on readwrite computed properties.
  bool PrintGetSetOnRWProperties = true;

  /// Whether to print *any* accessors on properties.
  bool PrintPropertyAccessors = true;

  /// Whether to print *any* accessors on subscript.
  bool PrintSubscriptAccessors = true;

  /// Whether to print the accessors of a property abstractly,
  /// i.e. always as:
  /// ```
  /// var x: Int { get set }
  /// ```
  /// rather than the specific accessors actually used to implement the
  /// property.
  ///
  /// Printing function definitions takes priority over this setting.
  bool AbstractAccessors = true;

  /// Whether to print a property with only a single getter using the shorthand
  /// ```
  /// var x: Int { return y }
  /// ```
  /// vs.
  /// ```
  /// var x: Int {
  ///   get { return y }
  /// }
  /// ```
  bool CollapseSingleGetterProperty = true;

  /// Whether to print the bodies of accessors in protocol context.
  bool PrintAccessorBodiesInProtocols = false;

  /// Whether to print type definitions.
  bool TypeDefinitions = false;

  /// Whether to print variable initializers.
  bool VarInitializers = false;

public:
  /// Whether to prefer printing TypeReprs instead of Types,
  /// if a TypeRepr is available.  This allows us to print the original
  /// spelling of the type name.
  ///
  /// \note This should be \c true when printing AST with the intention show
  /// it to the user.
  bool PreferTypeRepr = true;

  /// Whether to print fully qualified Types.
  bool FullyQualifiedTypes = false;

  /// Print fully qualified types if our heuristics say that a certain
  /// type might be ambiguous.
  bool FullyQualifiedTypesIfAmbiguous = false;

  /// Print fully qualified extended types if ambiguous.
  bool FullyQualifiedExtendedTypesIfAmbiguous = false;

  /// Whether to protocol-qualify DependentMemberTypes.
  bool ProtocolQualifiedDependentMemberTypes = false;

  /// If true, printed module names will use the "exported" name, which may be
  /// different from the regular name.
  ///
  /// \see FileUnit::getExportedModuleName
  bool UseExportedModuleNames = false;

  /// Use the original module name to qualify a symbol.
  bool UseOriginallyDefinedInModuleNames = false;

  /// Print Swift.Array and Swift.Optional with sugared syntax
  /// ([] and ?), even if there are no sugar type nodes.
  bool SynthesizeSugarOnTypes = false;

  /// If true, null types in the AST will be printed as "<null>". If
  /// false, the compiler will trap.
  bool AllowNullTypes = true;

  /// If true, the printer will explode a pattern like this:
  /// \code
  ///   var (a, b) = f()
  /// \endcode
  /// into multiple variable declarations.
  ///
  /// For this option to work correctly, \c VarInitializers should be
  /// \c false.
  bool ExplodePatternBindingDecls = false;

  /// If true, the printer will explode an enum case like this:
  /// \code
  ///   case A, B
  /// \endcode
  /// into multiple case declarations.
  bool ExplodeEnumCaseDecls = false;

  /// Whether to print implicit parts of the AST.
  bool SkipImplicit = false;

  /// Whether to print unavailable parts of the AST.
  bool SkipUnavailable = false;

  /// Whether to print synthesized extensions created by '@_nonSendable', even
  /// if SkipImplicit or SkipUnavailable is set.
  bool AlwaysPrintNonSendableExtensions = true;

  bool SkipSwiftPrivateClangDecls = false;

  /// Whether to skip internal stdlib declarations.
  bool SkipPrivateStdlibDecls = false;

  /// Whether to skip underscored stdlib protocols.
  /// Protocols marked with @_show_in_interface are still printed.
  bool SkipUnderscoredStdlibProtocols = false;

  /// Whether to skip unsafe C++ class methods that were renamed
  /// (e.g. __fooUnsafe). See IsSafeUseOfCxxDecl.
  bool SkipUnsafeCXXMethods = false;

  /// Whether to skip extensions that don't add protocols or no members.
  bool SkipEmptyExtensionDecls = true;

  /// Whether to print attributes.
  bool SkipAttributes = false;

  /// Whether to print keywords like 'func'.
  bool SkipIntroducerKeywords = false;

  /// Whether to print destructors.
  bool SkipDeinit = false;

  /// Whether to skip printing 'import' declarations.
  bool SkipImports = false;

  /// Whether to skip over the C++ inline namespace when printing its members or
  /// when printing it out as a qualifier.
  bool SkipInlineCXXNamespace = false;

  /// Whether to skip printing overrides and witnesses for
  /// protocol requirements.
  bool SkipOverrides = false;

  /// Whether to skip placeholder members.
  bool SkipMissingMemberPlaceholders = true;

  /// Whether to print a long attribute like '\@available' on a separate line
  /// from the declaration or other attributes.
  bool PrintLongAttrsOnSeparateLines = false;

  bool PrintImplicitAttrs = true;

  /// Whether to desugar the constraint for an existential type.
  bool DesugarExistentialConstraint = false;

  /// Whether to skip keywords with a prefix of underscore such as __consuming.
  bool SkipUnderscoredKeywords = false;

  /// Prints type variables and unresolved types in an expanded notation
  /// suitable for debugging.
  bool PrintTypesForDebugging = false;

  /// Whether this print option is for printing .swiftinterface file
  bool IsForSwiftInterface = false;

  /// Whether to print generic requirements in a where clause.
  bool PrintGenericRequirements = true;

  /// Whether to print generic signatures with inverse requirements (ie,
  /// ~Copyable noting the absence of Copyable) or the internal desugared form
  /// (where the implicit Copyable conformance is spelled explicitly).
  bool PrintInverseRequirements = false;

  /// Whether to print the internal layout name instead of AnyObject, etc.
  bool PrintInternalLayoutName = false;

  /// Suppress emitting @available(*, noasync)
  bool SuppressNoAsyncAvailabilityAttr = false;

  /// Whether to print the \c{/*not inherited*/} comment on factory
  /// initializers.
  bool PrintFactoryInitializerComment = true;

public:
  /// Whether to print decl attributes that are only used internally,
  /// such as _silgen_name, transparent, etc.
  bool PrintUserInaccessibleAttrs = true;

  /// Whether to limit ourselves to printing only the "current" set of members
  /// in a nominal type or extension, which is semantically unstable but can
  /// prevent printing from doing "extra" work.
  bool PrintCurrentMembersOnly = false;

  /// Whether to suppress printing of custom attributes that are expanded
  /// macros.
  bool SuppressExpandedMacros = true;

  /// Whether we're supposed to slap a @rethrows on `AsyncSequence` /
  /// `AsyncIteratorProtocol` for backward-compatibility reasons.
  bool AsyncSequenceRethrows = false;

  /// Suppress the @isolated(any) attribute.
  bool SuppressIsolatedAny = false;

  /// Suppress 'isolated' and '#isolation' on isolated parameters with optional
  /// type.
  bool SuppressOptionalIsolatedParams = false;

  /// Suppress Noncopyable generics.
  bool SuppressNoncopyableGenerics = false;

  /// Suppress printing of `borrowing` and `consuming`.
  bool SuppressNoncopyableOwnershipModifiers = false;

public:
  /// Whether to print storage representation attributes on types, e.g.
  /// '@sil_weak', '@sil_unmanaged'.
  bool PrintStorageRepresentationAttrs = false;

  /// Whether to print 'static' or 'class' on static decls.
  bool PrintStaticKeyword = true;

  /// Whether to print 'mutating', 'nonmutating', or '__consuming' keyword on
  /// specified decls.
  bool PrintSelfAccessKindKeyword = true;

  /// Whether to print 'override' keyword on overridden decls.
  bool PrintOverrideKeyword = true;

  /// Whether to print access control information on all value decls.
  bool PrintAccess = false;

  /// If \c PrintAccess is true, this determines whether to print
  /// 'internal' keyword.
  bool PrintInternalAccessKeyword = true;

  /// Print all decls that have at least this level of access.
  VisibilityLevel AccessFilter = VisibilityLevel::Private;

  /// Print IfConfigDecls.
  bool PrintIfConfig = true;

  /// Whether we are printing for sil.
  bool PrintForSIL = false;

  /// Whether we are printing part of SIL body.
  bool PrintInSILBody = false;

  /// Whether to use an empty line to separate two members in a single decl.
  bool EmptyLineBetweenDecls = false;

  /// Whether to print empty members of a declaration on a single line, e.g.:
  /// ```
  /// extension Foo: Bar {}
  /// ```
  bool PrintEmptyMembersOnSameLine = false;

  /// Whether to print the extensions from conforming protocols.
  bool PrintExtensionFromConformingProtocols = false;

  /// Whether to always try and print parameter labels. If present, print the
  /// external parameter name. Otherwise try printing the internal name as
  /// `_ <internalName>`, if an internal name exists. If neither an external nor
  /// an internal name exists, only print the parameter's type.
  bool AlwaysTryPrintParameterLabels = false;

public:
  /// Whether to print the default argument value string
  /// representation.
  bool PrintDefaultArgumentValue = true;

  /// Whether to print "_" placeholders for empty arguments.
  bool PrintEmptyArgumentNames = true;

  /// Whether to print documentation comments attached to declarations.
  /// Note that this may print documentation comments from related declarations
  /// (e.g. the overridden method in the superclass) if such comment is found.
  bool PrintDocumentationComments = false;

  /// When true, printing interface from a source file will print the original
  /// source text for applicable declarations, in order to preserve the
  /// formatting.
  bool PrintOriginalSourceText = false;

  /// When printing a type alias type, whether print the underlying type instead
  /// of the alias.
  bool PrintTypeAliasType = false;

  /// Print the definition of a macro, e.g. `= #externalMacro(...)`.
  bool PrintMacroDefinitions = true;

  /// Use aliases when printing references to modules to avoid ambiguities
  /// with types sharing a name with a module.
  bool AliasModuleNames = false;

  /// Whether to always print explicit `Pack{...}` around pack
  /// types.
  ///
  /// This is set to \c false for diagnostic arguments.
  bool PrintExplicitPackTypes = true;

public:
  /// Retrieve the set of options suitable for diagnostics printing.
  static PrintOptions CreateForDiagnostics(VisibilityLevel accessFilter,
                                           bool printFullConvention) {
    PrintOptions result = CreateForVerbose();
    result.PrintAccess = true;
    result.Indent = 4;
    /*
        result.FullyQualifiedTypesIfAmbiguous = true;
        result.SynthesizeSugarOnTypes = true;
        result.PrintUserInaccessibleAttrs = false;
        result.PrintImplicitAttrs = false;
        result.ExcludeAttrList.push_back(DeclAttrKind::Exported);
        result.ExcludeAttrList.push_back(DeclAttrKind::Inline);
        result.ExcludeAttrList.push_back(DeclAttrKind::Optimize);
        result.ExcludeAttrList.push_back(DeclAttrKind::Rethrows);
        result.PrintOverrideKeyword = false;
        result.AccessFilter = accessFilter;
        result.PrintIfConfig = false;
        result.ShouldQualifyNestedDeclarations =
            QualifyNestedDeclarations::TypesOnly;
        result.PrintDocumentationComments = false;
        result.PrintCurrentMembersOnly = true;
        if (printFullConvention)
          result.PrintFunctionRepresentationAttrs =
              PrintOptions::FunctionRepresentationMode::Full;

    */

    return result;
  }

  /// Retrieve the set of options for verbose printing to users.
  static PrintOptions CreateForVerbose() {
    PrintOptions result;
    result.TypeDefinitions = true;
    result.VarInitializers = true;
    result.PrintDocumentationComments = true;
    result.PrintLongAttrsOnSeparateLines = true;
    result.AlwaysTryPrintParameterLabels = true;
    return result;
  }

  /// The print options used for formatting diagnostic arguments.
  static PrintOptions CreateForDiagnosticArguments() {
    PrintOptions result;
    result.PrintExplicitPackTypes = false;
    return result;
  }
};
} // namespace stone
#endif
