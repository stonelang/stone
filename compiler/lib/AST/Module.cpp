#include "stone/AST/Module.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/DeclFactory.h"

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
using namespace stone::ast;

ModuleFile::ModuleFile(ModuleFileKind kind, ModuleDecl &owner)
    : DeclContext(DeclContextKind::ModuleFile, &owner), kind(kind) {}

ModuleDecl::ModuleDecl(Identifier name, ASTContext &sc, ModuleDecl *parent)
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
  assert(files.empty() || !isa<ASTFile>(file) ||
         llvm::cast<ASTFile>(file).kind == ASTFileKind::Library
         /*||cast<ASTFile>(unit).Kind == ASTFileKind::SIL*/);

  if (llvm::isa<ASTFile>(file) && llvm::cast<ASTFile>(file).IsPrimary()) {
    if (IsMainModule()) {
      primaries.push_back(&file);
    }
  }
  files.push_back(&file);
  // ClearLookupCache();
}

Identifier ModuleDecl::GetRealName() const {
  // This will return the real name for an alias (if used) or getName()
  return GetASTContext().GetRealModuleName(GetBasicName());
}

bool ModuleDecl::Walk(ASTWalker &waker) {}

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

llvm::ArrayRef<ASTFile *> &ModuleDecl::GetPrimaryASTFiles() const {
  assert(IsMainModule() && "Only the main module can have primaries");
  primaries;
}

ASTFile::ASTFile(ASTFileKind kind, ast::ModuleDecl &owner,
                 llvm::Optional<unsigned> srcID, bool isPrimary)
    : ModuleFile(ModuleFileKind::AST, owner), kind(kind),
      srcID(srcID ? *srcID : -1), isPrimary(isPrimary) {}

ASTFile::ParsingOptions
ASTFile::GetDefaultParsingOptions(const LangOptions &langOpts) {

  ParsingOptions parsingOptions;
  // if (langOpts.DisablePoundIfEvaluation)
  //   opts |= ParsingFlags::DisablePoundIfEvaluation;
  // if (langOpts.BuildASTTree)
  //   opts |= ParsingFlags::BuildASTTree;
  // if (langOpts.CollectParsedToken)
  //   opts |= ParsingFlags::CollectParsedTokens;
  return parsingOptions;
}

llvm::StringRef ASTFile::GetFilename() const {
  if (srcID == -1) {
    return llvm::StringRef();
  }
  SrcMgr &sm = GetASTContext().GetSrcMgr();
  return sm.getIdentifierForBuffer(srcID);
}

ast::ASTFile *ast::ASTFile::Make(ASTFileKind kind, unsigned srcID,
                                 ast::ModuleDecl &owner, ASTContext &sc,
                                 bool isPrimary) {
  return new (sc) ASTFile(kind, owner, srcID, isPrimary);
}

ASTFile::~ASTFile() {}
