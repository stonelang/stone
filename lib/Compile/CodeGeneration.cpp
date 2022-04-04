#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMContext.h"
#include "stone/Compile/LangInstance.h"
#include "stone/Compile/LangListener.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/Gen.h"
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

struct LangInstance::CodeGeneration final {

  friend LangInstance;
  LangInstance &lang;

  CodeGeneration(LangInstance &lang);
  ~CodeGeneration();

  void Compile(const CodeAnalysis &codeAnalysis);

  /// Generate the IR for an entire module
  llvm::Module *GenIR(syn::Module &sf, CodeGenContext &cc);

  /// Generate IR a single SyntaxFile
  llvm::Module *GenIR(syn::SyntaxFile &sf, CodeGenContext &cc);

  /// Generate Object file
  void GenObject(unsigned srcID, llvm::Module *mod, CodeGenContext &cc);

  /// Generate Object file
  void GenBitCode();

  /// Generates a 'test.stonem' file
  void GenModule();
};

LangInstance::CodeGeneration::CodeGeneration(LangInstance &lang) : lang(lang) {}

LangInstance::CodeGeneration::~CodeGeneration() {}

inline LangInstance::CodeGeneration &LangInstance::GetCodeGeneration() {
  auto pointer = reinterpret_cast<char *>(const_cast<LangInstance *>(this));
  auto offset = llvm::alignAddr((void *)sizeof(*this),
                                llvm::Align(alignof(CodeGeneration)));
  return *reinterpret_cast<LangInstance::CodeGeneration *>(pointer + offset);
}

void LangInstance::PerformCodeGeneration(const CodeAnalysis &codeAnalysis) {
  GetCodeGeneration().Compile(codeAnalysis);
}

void LangInstance::CodeGeneration::Compile(const CodeAnalysis &codeAnalysis) {

  assert(lang.GetLangInvocation().CanCodeGen());

  // We are performing some low leverl code generation
  CodeGenContext cgc(stone::GetLLVMContext(),
                     lang.GetLangInvocation().GetCodeGenOptions());

  // At this point, we much generate IR for all succeeding modes

  // TODO:
  /// Choose what we are going to do
  // auto llvmMod = stone::GenIR(*syntaxFile, cgc);

  // auto mod = stone::GenIR(GetMainModule(), cgc);

  if (lang.GetLangInvocation().GetMode().IsEmitIR()) {
    // EmitIR()
    return;
  }

  if (lang.GetLangInvocation().GetMode().IsNone() ||
      lang.GetLangInvocation().GetMode().IsEmitObject()) {
    // GenObject(srcID, llvmMod, cgc);
    return;
  }
}
llvm::Module *LangInstance::CodeGeneration::GenIR(syn::SyntaxFile &sf,
                                                  CodeGenContext &cc) {
  return nullptr;
}

llvm::Module *LangInstance::CodeGeneration::GenIR(syn::Module &mod,
                                                  CodeGenContext &cc) {
  return nullptr;
}
void LangInstance::OptimizeIR(llvm::Module *mod) {
  // stone::OptimizeIR
}

void LangInstance::CodeGeneration::GenObject(const unsigned srcID,
                                             llvm::Module *mod,
                                             CodeGenContext &cc) {
  /// TODO: This is the only time we should perform a lookup
  // auto outputFile = lang.GetLangInvocation().ComputeOutputFile(srcID);
  // auto result GenObject(cgc GetSyntaxContext(), outputFile.get());
}
