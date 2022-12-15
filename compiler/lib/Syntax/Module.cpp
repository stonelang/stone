#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/DeclFactory.h"

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
    : DeclContext(DeclContextKind::Module),
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

Identifier ModuleDecl::GetRealName() const {
  // This will return the real name for an alias (if used) or getName()
  return GetSyntaxContext().GetRealModuleName(GetBasicName());
}

bool ModuleDecl::Walk(SyntaxWalker &waker) {}

bool DeclContext::IsModuleContext() const {
  if (auto D = ToDecl()) {
    return ModuleDecl::classof(D);
  }
  return false;
}

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

SyntaxFile::ParsingOptions
SyntaxFile::GetDefaultParsingOptions(const LangOptions &langOpts) {

  ParsingOptions parsingOptions;
  // if (langOpts.DisablePoundIfEvaluation)
  //   opts |= ParsingFlags::DisablePoundIfEvaluation;
  // if (langOpts.BuildSyntaxTree)
  //   opts |= ParsingFlags::BuildSyntaxTree;
  // if (langOpts.CollectParsedToken)
  //   opts |= ParsingFlags::CollectParsedTokens;
  return parsingOptions;
}

syn::SyntaxFile *syn::SyntaxFile::Make(SyntaxFileKind kind,unsigned srcID,
                                       syn::ModuleDecl &owner,
                                       SyntaxContext &sc, 
                                       bool isPrimary) {
  return new (sc) SyntaxFile(kind, owner, srcID, isPrimary);
}

SyntaxFile::~SyntaxFile() {}
