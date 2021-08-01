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
  void CompileFile(CompilableFile &cf);
  void ParseFile(CompilableFile &cf);
  void TypeCheckFile(syn::SyntaxFile *sf);
  void EmitIR(syn::SyntaxFile *sf);
  void EmitObject(llvm::Module *m);
};

} // namespace stone
#endif
