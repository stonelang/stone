#ifndef STONE_SYNTAX_MODULE_H
#define STONE_SYNTAX_MODULE_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Basic/Printable.h"
#include "stone/Basic/STDTypeAlias.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Scope.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxWalker.h"
#include "llvm/ADT/SmallVector.h"

namespace stone {
namespace syn {

class Module;

static inline unsigned AlignOfModuleFile();

enum class ModuleFileKind : uint8_t { Source, Builtin };

class ModuleFile : public DeclContext, public SyntaxAllocation<ModuleFile> {
private:
  ModuleFileKind kind;

public:
  ModuleFile(ModuleFileKind kind, ModuleDecl &owner);

public:
  ModuleFileKind GetKind() const { return kind; }

public:
  using SyntaxAllocation<ModuleFile>::operator new;
  using SyntaxAllocation<ModuleFile>::operator delete;
};

enum class SyntaxFileKind : uint8_t {
  None,
  // .stone file without 'Main'
  Library,
  // .stone file with 'Main entry' //TODO: rename to Executable
  Main
};

enum class SyntaxFileStage : uint8_t {
  None = 0,
  ImportsResolved,
  TypeChecked,
};

class SyntaxFile final : public ModuleFile /*, public Printable*/ {
private:
  friend SyntaxContext;
  // llvm::NullablePtr<Scope> scope = nullptr;

  /// A unique identifier representing this file; used to mark private decls
  /// within the file to keep them from conflicting with other files in the
  /// same module.
  // mutable Identifier privateDiscriminator;

  // The ID for the memory buffer containing this file's source.
  ///
  /// May be -1, to indicate no association with a buffer.

  int srcID;

  /// Whether this is a primary source file which we'll be generating code for.
  bool isPrimary;

  /// fun Main() -> int {}
  bool hasMainFun;

public:
  /// Flags that direct how the source file is parsed.
  enum class ParsingFlags : UInt8 {
    /// Whether to disable delayed parsing for nominal type, extension, and
    /// function bodies.
    ///
    /// If set, type and function bodies will be parsed eagerly. Otherwise they
    /// will be lazily parsed when their contents is queried. This lets us avoid
    /// building AST nodes when they're not needed.
    ///
    /// This is set for primary files, since we want to type check all
    /// declarations and function bodies anyway, so there's no benefit in lazy
    /// parsing.
    DisableDelayedBodies = 1 << 0,

    /// Whether to disable evaluating the conditions of #if decls.
    ///
    /// If set, #if decls are parsed as-is. Otherwise, the bodies of any active
    /// clauses are hoisted such that they become sibling nodes with the #if
    /// decl.
    ///
    /// FIXME: When condition evaluation moves to a later phase, remove this
    /// and the associated language option.
    DisablePoundIfEvaluation = 1 << 1,

    /// Whether to build a syntax tree.
    BuildSyntaxTree = 1 << 2,

    /// Whether to save the file's parsed tokens.
    CollectParsedTokens = 1 << 3,

    /// Whether to compute the interface hash of the file.
    EnableInterfaceHash = 1 << 4,

    /// Whether to suppress warnings when parsing. This is set for secondary
    /// files, as they get parsed multiple times.
    SuppressWarnings = 1 << 5,
  };
  using ParsingOptions = OptionSet<ParsingFlags>;

  /// Retrieve the parsing options specified in the LangOptions.
  static ParsingOptions GetDefaultParsingOptions(const LangOptions &langOpts);

public:
  SyntaxFileKind kind = SyntaxFileKind::None;
  SyntaxFileStage stage = SyntaxFileStage::None;

  std::vector<Decl *> Decls;

public:
  SyntaxFile(SyntaxFileKind kind, syn::ModuleDecl& owner,
             llvm::Optional<unsigned> srcID, bool isPrimary = false);

  ~SyntaxFile();

public:
  bool IsPrimary() { return isPrimary; }
  unsigned GetSrcID() { return srcID; }

  bool HasMainFun() { return hasMainFun; }
  void SetHasMainFun(bool status = false) { status = hasMainFun; }

  void AddTopLevelDecl(Decl *d) {
    // Force decl parsing if we haven't already.
    //(void)GetTopLevelDecls();
    Decls.push_back(d);
  }

  /// Retrieves an immutable view of the list of top-level decls in this file.
  llvm::ArrayRef<Decl *> GetDecls() const;

  // void Print(llvm::raw_ostream &os, const PrintingPolicy &policy) const
  // override;

  // void Dump(raw_ostream &os, bool parseIfNeeded = false) const;

  /// Pretty-print the contents of this source file.
  ///
  /// \param Printer The AST printer used for printing the contents.
  /// \param PO Options controlling the printing process.
  // void Print(SyntaxPrinter &printer, const SyntaxPrintOptions &PO);
  // void Print(raw_ostream &stream, const SyntaxPrintOptions &PO);

public:
  static syn::SyntaxFile *Make(SyntaxFileKind kind, unsigned srcID, ModuleDecl &owner,
                               SyntaxContext &tc, bool isPrimary = false);

  static bool classof(const ModuleFile *file) {
    return file->GetKind() == ModuleFileKind::Source;
  }
};

class BuiltinFile final : public ModuleFile {
public:
};

class ModuleDecl final : public DeclContext,
                         public TypeDecl,
                         public SyntaxAllocation<ModuleDecl> {

  /// The ABI name of the module, if it differs from the module name.
  mutable Identifier moduleABIName;

public:
  ModuleDecl(Identifier name, SyntaxContext &tc);

public:
  using syn::Decl::GetSyntaxContext;
  llvm::SmallVector<ModuleFile *, 2> files;

public:
  llvm::ArrayRef<ModuleFile *> GetFiles() { return files; }
  llvm::ArrayRef<const ModuleFile *> GetFiles() const {
    return {files.begin(), files.size()};
  }
  void AddFile(ModuleFile &file);

  SyntaxFile &GetMainSyntaxFile() const;
  ModuleFile &GetMainFile(ModuleFileKind kind) const;

  /// For the main module, retrieves the list of primary source files being
  /// compiled, that is, the files we're generating code for.
  llvm::ArrayRef<SyntaxFile *> GetPrimarySyntaxFiles() const;

public:
  /// \returns true if this module is the "stone" standard library module.
  bool IsSTD() const;

  /// \returns true if this module is the "builtin" module.
  bool IsBuiltin() const;

  bool Walk(SyntaxWalker &waker);

  /// Retrieve the ABI name of the module, which is used for metadata and
  /// mangling.
  Identifier GetABIName() const;
  // /// Set the ABI name of the module;
  // void SetABIName(Identifier name) {
  //   moduleABIName = name;
  //}
  /// Retrieve the actual module name of an alias used for this module (if any).
  ///
  /// For example, if '-module-alias Foo=Bar' is passed in when building the
  /// main module, and this module is (a) not the main module and (b) is named
  /// Foo, then it returns the real (physically on-disk) module name Bar.
  ///
  /// If no module aliasing is set, it will return getName(), i.e. Foo.
  Identifier GetRealName() const;

  // TODO: Defaulting to true for now
  bool IsMainModule() const { return Bits.ModuleDecl.IsMainModule; }

public:
  static bool classof(const DeclContext *DC) {
    if (auto D = DC->ToDecl()) {
      return classof(D);
    }
    return false;
  }
  static bool classof(const Decl *D) {
    return D->GetKind() == DeclKind::Module;
  }
};

static inline unsigned AlignOfModuleFile() { return alignof(ModuleFile &); }

} // namespace syn
} // namespace stone

#endif
