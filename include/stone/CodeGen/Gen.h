#ifndef STONE_CODEGEN_BACKEND_H
#define STONE_CODEGEN_BACKEND_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/OutputFile.h"

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
} // namespace syn
namespace codegen {
class CodeGenOptions;
class CodeGenModule;
} // namespace codegen

std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(const codegen::CodeGenOptions &genOpts,
                    syn::TreeContext &basic);

// TODO: remove GenModuleProfile
llvm::Module *GenIR(syn::Module *langMod, const Basic &basic,
                    const codegen::CodeGenOptions &genOpts,
                    const OutputFile *output);

bool GenObject(llvm::Module *llvmMod, const codegen::CodeGenOptions &genOpts,
               syn::TreeContext &basic, const OutputFile *output);

void GenModule();

void GenAssembly();

void GenBitCode();

void GenLibrary();

} // namespace stone
#endif
