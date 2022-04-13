#ifndef STONE_COMPILE_TARGETMACHINE_H
#define STONE_COMPILE_TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include <memory>

namespace stone {
class CodeGenOptions;
namespace syn {
class SyntaxContext;
}
std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(const CodeGenOptions &codeGenOpts, syn::SyntaxContext &tc);

} // namespace stone

#endif