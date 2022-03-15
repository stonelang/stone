#ifndef STONE_GEN_GEN_H
#define STONE_GEN_GEN_H

#include "stone/Core/LLVM.h"
#include "stone/Core/OutputFile.h"
#include "stone/Syntax/TreeContext.h"

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
class TreeContext;
class Module;
class SyntaxFile;
} // namespace syn

class CodeGenOptions;
class CodeGenContext;

std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(const CodeGenOptions &genOpts, syn::TreeContext &tc);

std::unique_ptr<llvm::Module> GenIR(CodeGenContext &cgc, syn::SyntaxFile &sf,
                                    const Context &ctx,
                                    const OutputFile *output);

std::unique_ptr<llvm::Module> GenIR(CodeGenContext &cgc, syn::Module &mod,
                                    const Context &ctx,
                                    const OutputFile *output);

bool GenObject(CodeGenContext &cgc, syn::TreeContext &tc,
               const OutputFile *output);

// void GenModule();

// void GenAssembly();

// void GenBitCode();

// void GenLibrary();

} // namespace stone
#endif
