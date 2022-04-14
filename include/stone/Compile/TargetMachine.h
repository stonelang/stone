#ifndef STONE_COMPILE_TARGETOPTIONS_H
#define STONE_COMPILE_TARGETOPTIONS_H

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
namespace syn {
class SyntaxContext;
}
std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(const CodeGenOptions &codeGenOpts,
                    const LangOptions &langOpts, syn::SyntaxContext &sc,
                    llvm::Module &llvmModule);

} // namespace stone

#endif