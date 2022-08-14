#ifndef STONE_GEN_GEN_H
#define STONE_GEN_GEN_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/OutputFile.h"
#include "stone/Gen/IRCodeGenResult.h"
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
class EmitPipeline;

namespace syn {
class SyntaxContext;
class Module;
class SyntaxFile;
} // namespace syn

class CodeGenOptions;
class CodeGenContext;
class IRCodeGenResult;

std::unique_ptr<IRCodeGenResult> GenIR(CodeGenContext &cgc, syn::SyntaxFile &sf,
                                       const Context &ctx,
                                       const OutputFile *output);

std::unique_ptr<IRCodeGenResult> GenIR(CodeGenContext &cgc, syn::Module &mod,
                                       const Context &ctx,
                                       const OutputFile *output);

stone::Error GenNative(CodeGenContext &cgc, syn::SyntaxContext &tc,
                       IRCodeGenResult &result, const OutputFile *output);

// void GenModule();

// void GenAssembly();

// void GenBitCode();

// void GenLibrary();

} // namespace stone
#endif
