#ifndef STONE_COMPILE_TARGETMACHINE_H
#define STONE_COMPILE_TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include <memory>

namespace llvm {
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {
class CodeGenOptions;
class LangOptions;
class DiagnosticEngine;
namespace syn {
class SyntaxContext;
}
std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(DiagnosticEngine &de, const CodeGenOptions &codeGenOpts,
                    const LangOptions &langOpts, syn::SyntaxContext &sc,
                    llvm::Module &llvmModule);

} // namespace stone

#endif