#ifndef STONE_COMPILE_COMPILABLEITEM_H
#define STONE_COMPILE_COMPILABLEITEM_H

#include "stone/Compile/CompilableFile.h"
#include "stone/Utils/OutputFile.h"

namespace llvm {
class Module;
} // namespace llvm

namespace stone {
class Compiler;

namespace syn {
class SyntaxFile;
}

class CompilingScope final {
public:
  CompilingScope(const CompilingScope &) = delete;
  CompilingScope(CompilingScope &&) = delete;
  CompilingScope &operator=(const CompilingScope &) = delete;
  CompilingScope &operator=(CompilingScope &&) = delete;

public:
  CompilingScope() {}
  ~CompilingScope() {}

public:
  void Enter();
  void Exit();
};

class CompilableItem final {

  Compiler &compiler;
  syn::SyntaxFile &sf;
  const CompilableFile &input;
  OutputFile *output = nullptr;

public:
  // TODO: May use CompilingScope insteat
  struct Stage {};

public:
  CompilableItem(const CompilableItem &) = delete;
  CompilableItem(CompilableItem &&) = delete;
  CompilableItem &operator=(const CompilableItem &) = delete;
  CompilableItem &operator=(CompilableItem &&) = delete;

public:
  CompilableItem(const CompilableFile &input, Compiler &compiler,
                 syn::SyntaxFile &sf)
      : input(input), compiler(compiler), sf(sf) {}
  ~CompilableItem() {}

public:
  // TODO: We may want to remove this
  // void SetSyntaxFile(syn::SyntaxFile *s) { sf = s; }
  syn::SyntaxFile &GetSyntaxFile() { return sf; }

  const CompilableFile &GetCompilableFile() const { return input; }

  void SetOutputFile(OutputFile *o) { output = o; }
  OutputFile *GetOutputFile() const { return output; }
  Compiler &GetCompiler() { return compiler; }

  bool CanOutput();
  void CreateOutputFile();
  void PurgeOutputFile();

public:
};
} // namespace stone
#endif
