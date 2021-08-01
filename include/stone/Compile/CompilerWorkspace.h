#ifndef STONE_COMPILE_COMPILERWORKSPACE_H
#define STONE_COMPILE_COMPILERWORKSPACE_H

namespace llvm {
class Module;
}
namespace stone {
namespace syn {
class SyntaxFile;
}
class Compiler;
class CompilableFile;

class CompilerWorkspace {

  Compiler &compiler;

public:
  CompilerWorkspace(Compiler &compiler);
  ~CompilerWorkspace();

public:
  void BuildCompilableFiles();
  void CompileFiles();

private:
  void CompileFile(const CompilableFile &cf);
  void Parse(const CompilableFile &cf);

  void TypeCheck(syn::SyntaxFile *sf);
  void EmitIR(syn::SyntaxFile *sf);
  void EmitObject(llvm::Module *m);
  void EmitModule(llvm::Module *m);
};

} // namespace stone
#endif
