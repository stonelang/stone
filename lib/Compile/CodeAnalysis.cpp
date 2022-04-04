#include "stone/Compile/LangInstance.h"
#include "stone/Compile/LangListener.h"
#include "stone/Compile/Parse.h"
#include "stone/Compile/TypeCheck.h"
#include "stone/Core/Defer.h"
#include "stone/Core/LLVMContext.h"
#include "stone/Session/ModeKind.h"
#include "stone/Syntax/Module.h"
#include "llvm/IR/Module.h"

using namespace stone;

using stone::LangInstance;
using stone::LangListener;
using stone::ModeKind;
using stone::SyntaxListener;
using stone::syn::SyntaxFile;
using stone::syn::SyntaxFileKind;

struct LangInstance::CodeAnalysis final {

  friend LangInstance;
  LangInstance &lang;

  CodeAnalysis(LangInstance &lang);
  ~CodeAnalysis();

  void Compile(llvm::ArrayRef<SourceUnit *> sources);
  void Compile(SourceUnit &source);

  SyntaxFile *Parse(const unsigned srcID);

  void ResolveUse();

  /// Print out the syntax tree
  void EmitParse(syn::SyntaxFile *sf);

  /// Perform type-checking on the SyntaxFile
  void TypeCheckSyntaxFile(syn::SyntaxFile &sf);

  /// Perform type-checking on the entire module
  void TypeCheckModule(syn::Module *mod);

  /// Emit the syntax after performing type-checking
  void EmitSyntax(syn::SyntaxFile *sf);
};

LangInstance::CodeAnalysis::CodeAnalysis(LangInstance &lang) : lang(lang) {}

inline LangInstance::CodeAnalysis &LangInstance::GetCodeAnalysis() {
  auto pointer = reinterpret_cast<char *>(const_cast<LangInstance *>(this));
  auto offset = llvm::alignAddr((void *)sizeof(*this),
                                llvm::Align(alignof(CodeAnalysis)));
  return *reinterpret_cast<LangInstance::CodeAnalysis *>(pointer + offset);
}

LangInstance::CodeAnalysis::~CodeAnalysis() {}

void LangInstance::PerformCodeAnalysis(llvm::ArrayRef<SourceUnit *> sources) {
  GetCodeAnalysis().Compile(sources);
}

void LangInstance::CodeAnalysis::Compile(llvm::ArrayRef<SourceUnit *> sources) {
  for (auto source : sources) {
    assert(source);
    Compile(*source);
  }
  if (lang.GetLangInvocation().GetMode().JustParse()) {
    return;
  }
  if (lang.GetLangInvocation().GetTypeCheckMode() ==
      types::TypeCheckMode::WholeModule) {
    TypeCheckModule(nullptr /*TODO: get module*/);
  }
  if (lang.GetLangInvocation().GetMode().JustTypeCheck()) {
    // Do some things
    return;
  }
}
void LangInstance::CodeAnalysis::Compile(SourceUnit &source) {
  auto syntaxFile = Parse(source.GetSrcID());
  assert(syntaxFile);
  // Add to module

  // TODO: May not need this because we are going to add it to the main module.
  // source.SetSyntaxFile(syntaxFile);

  if (lang.GetLangInvocation().GetMode().IsParse()) {
    return;
  }
  if (lang.GetLangInvocation().GetMode().IsEmitParse()) {
    // lang.EmitParse(syntaxFile);
    return;
  }
  if (lang.GetLangInvocation().GetTypeCheckMode() ==
      types::TypeCheckMode::EachFile) {
    TypeCheckSyntaxFile(*syntaxFile);
  }
  if (lang.GetLangInvocation().GetMode().IsTypeCheck()) {
    return;
  }
  if (lang.GetLangInvocation().GetMode().IsEmitSyntax()) {
    // lang.EmitSyntax(*sntaxFile)
  }
}
SyntaxFile *LangInstance::CodeAnalysis::Parse(const unsigned srcID) {
  // TODO: You are not always creating a Library
  auto sf = SyntaxFile::Make(SyntaxFileKind::Library,
                             *lang.GetModuleSystem().GetMainModule(),
                             lang.GetSyntax().GetSyntaxContext(), srcID);

  syn::ParseSyntaxFile(*sf, lang.GetSyntax());
  return sf;
}

void LangInstance::CodeAnalysis::TypeCheckSyntaxFile(SyntaxFile &sf) {
  assert(sf.stage == syn::SyntaxFileStage::AtImports);
  types::TypeCheckSyntaxFile(sf,
                             lang.GetLangInvocation().GetTypeCheckerOptions());
}
/// Perform type-checking on the entire module
void LangInstance::CodeAnalysis::TypeCheckModule(syn::Module *mod) {
  assert(mod && "Null 'syn::Module'");
  for (auto mf : mod->GetFiles()) {
    if (auto sf = llvm::dyn_cast<SyntaxFile>(mf)) {
      TypeCheckSyntaxFile(*sf);
    }
  }
}

void LangInstance::CodeAnalysis::ResolveUse() {}