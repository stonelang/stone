#include "stone/AST/Module.h"
#include "stone/AST/ASTContext.h"

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

ModuleFile::ModuleFile(ModuleFileKind kind, ModuleDecl &owner)
    : DeclContext(DeclContextKind::ModuleFile, &owner), kind(kind) {}

ModuleDecl::ModuleDecl(DeclState *DS, ASTContext &AC, ModuleDecl *parent)
    : DeclContext(DeclContextKind::ModuleDecl),
      TypeDecl(DeclKind::Module, DS, &AC), parent(parent) {

  // SetVisibilityLevel(VisibilityLevel::Public);

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
  assert(files.empty() || !isa<SourceFile>(file) ||
         llvm::cast<SourceFile>(file).kind == SourceFileKind::Library
         /*||cast<SourceFile>(unit).Kind == SourceFileKind::SIL*/);
  files.push_back(&file);
  // TODO: ClearLookupCache();
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

// TODO: improve one
llvm::ArrayRef<SourceFile *> ModuleDecl::GetPrimarySourceFiles() {
  assert(IsMainModule() && "Only the main module can have primary files!");
  llvm::SmallVector<SourceFile *, 8> primaries;
  for (auto *moduleFile : GetFiles()) {
    if (auto *sourceFile = llvm::dyn_cast<SourceFile>(moduleFile)) {
      if (sourceFile->IsPrimary()) {
        primaries.push_back(sourceFile);
      }
    }
  }
  return GetASTContext().AllocateCopy(primaries);
}

// void ModuleDecl::ForEachSourceFileInModule(
//     std::function<void(SourceFile &sourceFile)> notify) const {
//   // for (auto *moduleFile : GetFiles()) {
//   //   if (auto *sourceFile = llvm::dyn_cast<SourceFile>(moduleFile)) {
//   //     return notify(sourceFile);
//   //   }
//   // }
// }

// Status ModuleDecl::ForEachSourceFile(std::function<Status(SourceFile
// &sourceFile)> notify) {
//   for (auto *moduleFile : GetFiles()) {
//     if (auto *sourceFile = llvm::dyn_cast<SourceFile>(moduleFile)) {
//       if(notify(*sourceFile).IsError()){
//         return Status::Error();
//       }
//     }
//   }
//   return Status();
// }
SourceFile::SourceFile(SourceFileKind kind, ModuleDecl &owner,
                       std::optional<unsigned> srcID, bool isPrimary)
    : ModuleFile(ModuleFileKind::Syntax, owner), kind(kind),
      srcID(srcID ? *srcID : -1), isPrimary(isPrimary) {}

SourceFile::ParsingOptions
SourceFile::GetDefaultParsingOptions(const LangOptions &langOpts) {

  ParsingOptions parsingOptions;
  // if (langOpts.DisablePoundIfEvaluation)
  //   opts |= ParsingFlags::DisablePoundIfEvaluation;
  // if (langOpts.BuildSyntaxTree)
  //   opts |= ParsingFlags::BuildSyntaxTree;
  // if (langOpts.CollectParsedToken)
  //   opts |= ParsingFlags::CollectParsedTokens;
  return parsingOptions;
}

llvm::StringRef SourceFile::GetFilename() const {
  if (srcID == -1) {
    return llvm::StringRef();
  }
  SrcMgr &sm = GetASTContext().GetSrcMgr();
  return sm.getIdentifierForBuffer(srcID);
}

SourceFile *SourceFile::Create(SourceFileKind kind, unsigned srcID,
                               ModuleDecl &owner, ASTContext &astContext) {
  return new (astContext) SourceFile(kind, owner, srcID, false);
}

SourceFile *SourceFile::CreatePrimarySourceFile(SourceFileKind kind,
                                                unsigned srcID,
                                                ModuleDecl &owner,
                                                ASTContext &astContext) {
  return new (astContext) SourceFile(kind, owner, srcID, true);
}

llvm::ArrayRef<Decl *> SourceFile::GetTopLevelDecls() const {
  return topLevelDecls;
}

SourceFile::~SourceFile() {}
