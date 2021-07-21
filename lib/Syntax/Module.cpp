#include "stone/Syntax/Module.h"

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

void *ModuleFile::operator new(size_t bytes, TreeContext &tc,
                               unsigned alignment) {
  return tc.Allocate(bytes, alignment);
}

ModuleFile::ModuleFile(ModuleFileKind kind, Module &owner)
    : DeclContext(DeclContextKind::File, DeclKind::None, &owner), kind(kind) {}

Module::Module(Identifier &name, TreeContext &tc)
    : DeclContext(DeclContextKind::Decl, DeclKind::Module),
      TypeDecl(DeclKind::Module, SrcLoc(), nullptr /*TODO: pass DeclContext*/) {

  // TODO: SetDeclName(name);
}

void Module::AddFile(ModuleFile &file) {
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

bool Module::Walk(SyntaxWalker &waker) {}

SyntaxFile::SyntaxFile(SyntaxFileKind kind, syn::Module &owner,
                       const SrcID srcID, bool isPrimary)
    : ModuleFile(ModuleFileKind::Source, owner), kind(kind), srcID(srcID),
      isPrimary(isPrimary) {}

syn::SyntaxFile *syn::SyntaxFile::Make(SyntaxFileKind kind, syn::Module &owner,
                                       TreeContext &tc, SrcID srcID,
                                       bool isPrimary) {

  auto *syntaxFile = new (tc) SyntaxFile(kind, owner, srcID, isPrimary);
  return syntaxFile;
}

SyntaxFile::~SyntaxFile() {}
