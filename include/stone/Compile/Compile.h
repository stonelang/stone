#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class LangListener;

namespace lang {
int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            LangListener *listener = nullptr);

// using FrontendCallback = llvm::function_ref<void(
//     LangInstance &lang, std::unique_ptr<IRCodeGenResult> result)>;

// static void CompileFrontend(llvm::ArrayRef<SourceUnit *> &sources,
//                                 LangInstance &lang, FrontendCallback fcb) {}
// static void CompileBackend(LangInstance &lang,
//                                std::unique_ptr<IRCodeGenResult> result) {}
} // namespace lang

} // namespace stone
#endif
