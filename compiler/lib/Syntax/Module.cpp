#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxFactory.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/MD5.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/SaveAndRestore.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;
using namespace stone::syn;

ModuleFile::ModuleFile(ModuleFileKind kind, ModuleDecl &owner)
    : DeclContext(DeclContextKind::ModuleFile, &owner), kind(kind) {}

ModuleDecl::ModuleDecl(Identifier name, SyntaxContext &sc)
    : DeclContext(DeclContextKind::Module, nullptr),
      TypeDecl(DeclKind::Module, name, SrcLoc(), Type(), &sc) {}

void ModuleDecl::AddFile(ModuleFile &file) {
  // If this is a LoadedFile, make sure it loaded without error.
  // assert(!(isa<LoadedFile>(newFile) &&
  //         cast<LoadedFile>(newFile).hadLoadError()));
  // Require Main and REPL files to be the first file added.
  assert(files.empty() || !isa<SyntaxFile>(file) ||
         cast<SyntaxFile>(file).kind == SyntaxFileKind::Library
         /*||cast<SyntaxFile>(unit).Kind == SyntaxFileKind::SIL*/);
  files.push_back(&file);
  // ClearLookupCache();
}

bool ModuleDecl::Walk(SyntaxWalker &waker) {}

llvm::ArrayRef<SyntaxFile *> ModuleDecl::GetPrimarySyntaxFiles() const {
  // auto &eval = GetASTContext().evaluator;
  // auto *mutableThis = const_cast<ModuleDecl *>(this);
  // return evaluateOrDefault(eval, PrimarySourceFilesRequest{mutableThis}, {});
  assert(false && "Not implemented");
}

SyntaxFile::SyntaxFile(SyntaxFileKind kind, syn::ModuleDecl &owner,
                       llvm::Optional<unsigned> srcID, bool isPrimary)
    : ModuleFile(ModuleFileKind::Source, owner), kind(kind),
      srcID(srcID ? *srcID : -1), isPrimary(isPrimary) {}

syn::SyntaxFile *syn::SyntaxFile::Make(SyntaxFileKind kind,
                                       syn::ModuleDecl &owner,
                                       SyntaxContext &sc, unsigned srcID,
                                       bool isPrimary) {
  return new (sc) SyntaxFile(kind, owner, srcID, isPrimary);
}

syn::SyntaxFile *SyntaxFileFactory::Create(SyntaxFileKind kind, unsigned srcID,
                                           syn::ModuleDecl &owner,
                                           SyntaxContext &sc, bool isPrimary) {
  return new (sc) SyntaxFile(kind, owner, srcID, isPrimary);
}

SyntaxFile::~SyntaxFile() {}
