#include "stone/Syntax/Module.h"
#include "stone/Syntax/DeclFactory.h"
#include "stone/Syntax/SyntaxContext.h"

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

ModuleDecl::ModuleDecl(Identifier name, SyntaxContext &sc, ModuleDecl *parent)
    : DeclContext(DeclContextKind::ModuleDecl),
      TypeDecl(DeclKind::Module, name, SrcLoc(), Type(), &sc), parent(parent) {

  SetAccessLevel(AccessLevel::Public);

  Bits.ModuleDecl.IsStaticLibrary = 0;
  Bits.ModuleDecl.IsTestingEnabled = 0;
  Bits.ModuleDecl.FailedToLoad = 0;
  Bits.ModuleDecl.RawResilienceStrategy = 0;
  Bits.ModuleDecl.HasResolvedImports = 0;
  Bits.ModuleDecl.PrivateImportsEnabled = 0;
  Bits.ModuleDecl.ImplicitDynamicEnabled = 0;
  Bits.ModuleDecl.IsSystemModule = 0;
  Bits.ModuleDecl.IsNonStoneModule = 0;
  Bits.ModuleDecl.IsMainModule = 0;
  Bits.ModuleDecl.HasIncrementalInfo = 0;
  Bits.ModuleDecl.HasHermeticSealAtLink = 0;
  Bits.ModuleDecl.IsConcurrencyChecked = 0;
}

void ModuleDecl::AddFile(ModuleFile &file) {
  // If this is a LoadedFile, make sure it loaded without error.
  // assert(!(isa<LoadedFile>(newFile) &&
  //         cast<LoadedFile>(newFile).hadLoadError()));
  // Require Main and REPL files to be the first file added.
  assert(files.empty() || !isa<SyntaxFile>(file) ||
         llvm::cast<SyntaxFile>(file).kind == SyntaxFileKind::Library
         /*||cast<SyntaxFile>(unit).Kind == SyntaxFileKind::SIL*/);

  if (llvm::isa<SyntaxFile>(file) && llvm::cast<SyntaxFile>(file).IsPrimary()) {
    if (IsMainModule()) {
      primaries.push_back(&file);
    }
  }
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
bool DeclContext::IsModuleFileContext() const {
  if (declContextKind == DeclContextKind::ModuleFile) {
    return true;
  }
  return IsModuleContext();
}

llvm::ArrayRef<SyntaxFile *> &ModuleDecl::GetPrimarySyntaxFiles() const {
  assert(IsMainModule() && "Only the main module can have primaries");
  primaries;
}

SyntaxFile::SyntaxFile(SyntaxFileKind kind, syn::ModuleDecl &owner,
                       llvm::Optional<unsigned> srcID, bool isPrimary)
    : ModuleFile(ModuleFileKind::Syntax, owner), kind(kind),
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

llvm::StringRef SyntaxFile::GetFilename() const {
  if (srcID == -1) {
    return llvm::StringRef();
  }
  SrcMgr &sm = GetSyntaxContext().GetSrcMgr();
  return sm.getIdentifierForBuffer(srcID);
}

syn::SyntaxFile *syn::SyntaxFile::Make(SyntaxFileKind kind, unsigned srcID,
                                       syn::ModuleDecl &owner,
                                       SyntaxContext &sc, bool isPrimary) {
  return new (sc) SyntaxFile(kind, owner, srcID, isPrimary);
}

SyntaxFile::~SyntaxFile() {}
