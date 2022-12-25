#ifndef STONE_GEN_GEN_H
#define STONE_GEN_GEN_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/OutputFile.h"
#include "stone/Syntax/SyntaxContext.h"

#include "llvm/Target/TargetMachine.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {
class CodeGenListener;

namespace syn {
class SyntaxContext;
class Module;
class SyntaxFile;
} // namespace syn

class CodeGenOptions;
class CodeGenContext;
class IRCodeGenResult;

void GenIR(CodeGenContext &cgc, syn::SyntaxFile &sf, const LangContext &ctx,
           llvm::StringRef outputFilename, CodeGenListener *listener = nullptr);

void GenIR(CodeGenContext &cgc, syn::ModuleDecl &mod, const LangContext &ctx,
           llvm::StringRef outputFilename, CodeGenListener *listener = nullptr);

void GenNative(CodeGenContext &cgc, syn::SyntaxContext &tc,
               llvm::StringRef outputFilename,
               CodeGenListener *listener = nullptr);

// void GenModule();

// void GenAssembly();

// void GenBitCode();

// void GenLibrary();

} // namespace stone
#endif
