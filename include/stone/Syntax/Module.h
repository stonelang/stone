#ifndef STONE_SYNTAX_MODULE_H
#define STONE_SYNTAX_MODULE_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Printable.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxScope.h"
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

enum class SyntaxFileKind : uint8_t { None, Library };
enum class SyntaxFileStage : uint8_t { None, AtImports, AtTypeCheck };

class SyntaxFile final : public ModuleFile /*, public Printable*/ {
private:
  friend SyntaxContext;
  // llvm::NullablePtr<SyntaxScope> scope = nullptr;

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

  // TODO: You may want const list
  List<Decl> topLevelDecls;

public:
  SyntaxFile(SyntaxFileKind kind, syn::Module &owner,
             llvm::Optional<unsigned> srcID, bool isPrimary = false);

  ~SyntaxFile();

public:
  bool IsPrimary() { return isPrimary; }
  unsigned GetSrcID() { return srcID; }

  bool HasMainFun() { return hasMainFun; }
  void SetHasMainFun(bool status = false) { status = hasMainFun; }

  void AddTopLevelDecl(Decl *topLvelDecl) { topLevelDecls.Add(topLvelDecl); }

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
                     public WalkableSyntax,
                     public SyntaxAllocation<Module> {
public:
  Module(Identifier name, SyntaxContext &tc);

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

  bool Walk(SyntaxWalker &waker) override;

public:
  static bool classof(const DeclContext *DC) {
    if (auto D = DC->GetAsDecl()) {
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
