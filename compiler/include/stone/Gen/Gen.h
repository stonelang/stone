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
class PrimaryFileSpecificPaths;

namespace syn {
class SyntaxContext;
class Module;
class SyntaxFile;
} // namespace syn

class CodeGenOptions;
class CodeGenContext;
class IRCodeGenResult;

void GenIR(CodeGenContext &cgc, llvm::StringRef moduleName, syn::SyntaxFile *sf,
           const PrimaryFileSpecificPaths specificPaths,
           CodeGenListener *listener = nullptr);

void GenIR(CodeGenContext &cgc, llvm::StringRef moduleName,
           syn::ModuleDecl *mod, const PrimaryFileSpecificPaths specificPaths,
           CodeGenListener *listener = nullptr);

void GenNative(CodeGenContext &cgc, syn::SyntaxContext &tc,
               llvm::StringRef outputFilename,
               CodeGenListener *listener = nullptr);

// void EmbedBitCode(llvm::Module *mod, const CodeGenOptions &codeGenOpts,
//                   llvm::MemoryBufferRef buffer);

// void EmbedObject(llvm::Module *mod, const CodeGenOptions &codeGenOpts);
} // namespace stone
#endif
