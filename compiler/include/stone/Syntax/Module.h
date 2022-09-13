#ifndef STONE_SYNTAX_MODULE_H
#define STONE_SYNTAX_MODULE_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Printable.h"
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
  ModuleFile(ModuleFileKind kind, Module &owner);

public:
  ModuleFileKind GetKind() const { return kind; }
};

enum class SyntaxFileKind : uint8_t {
  None,
  // .stone file without 'Main'
  Library,
  // .stone file with 'Main entry'
  Main
};

enum class SyntaxFileStage : uint8_t { None, AtImports, AtTypeCheck };

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
  SyntaxFileKind kind = SyntaxFileKind::None;
  SyntaxFileStage stage = SyntaxFileStage::None;

  llvm::Optional<std::vector<Decl *>> allDecls;

public:
  SyntaxFile(SyntaxFileKind kind, syn::Module &owner,
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
    allDecls->push_back(d);
  }

  /// Retrieves an immutable view of the list of top-level decls in this file.
  // llvm::ArrayRef<Decl *> GetTopLevelDecls() const;

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
  static syn::SyntaxFile *Make(SyntaxFileKind kind, Module &owner,
                               SyntaxContext &tc, unsigned srcID,
                               bool isPrimary = false);

  static bool classof(const ModuleFile *file) {
    return file->GetKind() == ModuleFileKind::Source;
  }
};

class BuiltinFile final : public ModuleFile {
public:
};

class Module final : public DeclContext,
                     public TypeDecl,
                     public SyntaxAllocation<Module> {

  /// The ABI name of the module, if it differs from the module name.
  mutable Identifier moduleABIName;

public:
  Module(Identifier *name, SyntaxContext &tc);

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

public:
  static bool classof(const DeclContext *DC) {
    if (auto D = DC->CastToDecl()) {
      return classof(D);
    }
    return false;
  }
  static bool classof(const Decl *D) {
    return D->GetKind() == DeclKind::Module;
  }
};

inline bool DeclContext::IsModuleContext() const {
  // TODO:
  // if (auto D = GetAsDecl()){
  //   return Module::classof(D);
  // }
  return false;
}

static inline unsigned AlignOfModuleFile() { return alignof(ModuleFile &); }

} // namespace syn
} // namespace stone

#endif
