#ifndef STONE_AST_MODULE_H
#define STONE_AST_MODULE_H

#include "stone/AST/ASTContext.h"
#include "stone/AST/ASTScope.h"
#include "stone/AST/ASTWalker.h"
#include "stone/AST/Decl.h"
#include "stone/AST/Identifier.h"
#include "stone/Basic/Basic.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Basic/Status.h"

#include "llvm/ADT/SmallVector.h"

namespace stone {

class ModuleDecl;
class SourceFile;
class ModuleFile;

static inline unsigned AlignOfModuleFile();

enum class ModuleOutputMode : uint8_t {
  None = 0,
  Single,
  Whole,
};

using ModuleDeclOrModuleFile = llvm::PointerUnion<ModuleDecl *, ModuleFile *>;

using ModuleNameAndOuptFileName = std::pair<llvm::StringRef, llvm::StringRef>;

class ModuleOptions final {
public:
  /// The map of aliases and underlying names of imported or referenced modules.
  llvm::StringMap<llvm::StringRef> moduleAliasMap;

  /// The name of the module that the invocation is building.
  std::string moduleName;
  bool HasModuleName() const { return !moduleName.empty(); }

  /// The ABI name of the module that the compile is building, to be used in
  /// mangling and metadata.
  std::string moduleABIName;

  /// The name of the library to link against when using this module.
  std::string moduleLinkName;

  ModuleOutputMode moduleOutputMode = ModuleOutputMode::None;
};

enum class ModuleFileKind : uint8_t { Syntax, Builtin };

class ModuleFile : public DeclContext, public ASTAllocation<ModuleFile> {
private:
  ModuleFileKind kind;

public:
  ModuleFile(ModuleFileKind kind, ModuleDecl &owner);

public:
  ModuleFileKind GetKind() const { return kind; }

public:
  // Efficiency override for DeclContext::getParentModule().
  ModuleDecl *GetParentModule() const {
    return const_cast<ModuleDecl *>(llvm::cast<ModuleDecl>(GetParent()));
  }

  static bool classof(const DeclContext *dc) {
    return dc->GetDeclContextKind() == DeclContextKind::ModuleFile;
  }

  using ASTAllocation<ModuleFile>::operator new;
  using ASTAllocation<ModuleFile>::operator delete;
};

enum class SourceFileKind : uint8_t {
  None,
  // .stone file without 'Main'
  Library,
  // .stone file with 'Main entry' //TODO: rename to Executable
  Main
};
// enum class IncludeDeclKind : uint8_t {
//   SourceFile = 0,
//   // .stone file without 'Main'
//   Directory,
// };
// class IncludeDecl : public NamedDecl {

// };

// class SourceFileDecl
class SourceFile final : public ModuleFile {
public:
  enum class Flags : uint8_t {
    None = 1 << 0,
    Parsed = 1 << 1,
    TypeChecked = 1 << 2,
  };
  using SourceFileStages = stone::OptionSet<SourceFile::Flags>;

private:
  friend ASTContext;
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
  enum class ParsingFlags : uint8 {
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
  SourceFileKind kind = SourceFileKind::None;
  SourceFileStages stages;

  std::vector<Decl *> topLevelDecls;

public:
  SourceFile(SourceFileKind kind, ModuleDecl &owner,
             std::optional<unsigned> srcID, bool isPrimary = false);

  ~SourceFile();

public:
  bool IsPrimary() const { return isPrimary; }
  unsigned GetSrcID() { return srcID; }

  bool HasMainFun() { return hasMainFun; }
  void SetHasMainFun(bool status = false) { status = hasMainFun; }

  void AddTopLevelDecl(Decl *d) {
    // Force decl parsing if we haven't already.
    //(void)GetTopLevelDecls();
    topLevelDecls.push_back(d);
  }

  /// Retrieves an immutable view of the list of top-level decls in this file.
  llvm::ArrayRef<Decl *> GetTopLevelDecls() const;

  /// If this buffer corresponds to a file on disk, returns the path.
  /// Otherwise, return an empty string.
  llvm::StringRef GetFilename() const;

  /// Retrieve the scope that describes this source file.
  ASTScope &GetScope();

  // void Print(llvm::raw_ostream &os, const PrintingPolicy &policy) const

  // void Dump(raw_ostream &os, bool parseIfNeeded = false) const;

  /// Pretty-print the contents of this source file.
  ///
  /// \param Printer The AST printer used for printing the contents.
  /// \param PO Options controlling the printing process.
  // void Print(ASTPrinter &printer, const SyntaxPrintOptions &PO);
  // void Print(raw_ostream &stream, const SyntaxPrintOptions &PO);

  void SetParsedStage() { stages |= SourceFile::Flags::Parsed; }
  bool HasParsed() { return stages.contains(SourceFile::Flags::Parsed); }
  void ClearParsedState();

  void SetTypeCheckedStage() { stages |= SourceFile::Flags::Parsed; }
  bool HasTypeChecked() {
    return stages.contains(SourceFile::Flags::TypeChecked);
  }
  void ClearTypeCheckedStage();

public:
  static SourceFile *Create(SourceFileKind kind, unsigned srcID,
                            ModuleDecl &owner, ASTContext &astContext);

  static SourceFile *CreatePrimarySourceFile(SourceFileKind kind,
                                             unsigned srcID, ModuleDecl &owner,
                                             ASTContext &astContext);

  static bool classof(const ModuleFile *file) {
    return file->GetKind() == ModuleFileKind::Syntax;
  }
  static bool classof(const DeclContext *dc) {
    return llvm::isa<ModuleFile>(dc) && classof(cast<ModuleFile>(dc));
  }
};

class BuiltinFile final : public ModuleFile {
public:
};

class ModuleDecl final : public DeclContext,
                         public TypeDecl,
                         public ASTAllocation<ModuleDecl> {

  // TODO: This is not yet implemented
  ModuleDecl *parent = nullptr;

  /// The ABI name of the module, if it differs from the module name.
  mutable Identifier moduleABIName;

public:
  ModuleDecl(DeclName name, ASTContext &tc, ModuleDecl *parent = nullptr);

public:
  using Decl::GetASTContext;
  llvm::SmallVector<ModuleFile *, 2> files;

  // TODO: llvm::SmallVector<ModuleFile *, 2> subModules;

public:
  llvm::ArrayRef<ModuleFile *> GetFiles() { return files; }
  llvm::ArrayRef<const ModuleFile *> GetFiles() const {
    return {files.begin(), files.size()};
  }
  void AddFile(ModuleFile &file);
  SourceFile &GetMainSourceFile() const;
  ModuleFile &GetMainFile(ModuleFileKind kind) const;

  /// For the main module, retrieves the list of primary source files being
  /// compiled, that is, the files we're generating code for.
  llvm::ArrayRef<SourceFile *> GetPrimarySourceFiles();

public:
  /// \returns true if this module is the "stone" standard library module.
  bool IsSTD() const;

  /// \returns true if this module is the "builtin" module.
  bool IsBuiltin() const;

  bool Walk(ASTWalker &waker);

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
  Status
  ForEachSourceFile(std::function<Status(SourceFile &sourceFile)> notify);

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

public:
  static ModuleDecl *Create(DeclName name, ASTContext &astContext);
  static ModuleDecl *CreateMainModule(DeclName name, ASTContext &astContext);
  static inline unsigned AlignOfModuleFile() { return alignof(ModuleFile &); }
};

} // namespace stone

#endif
