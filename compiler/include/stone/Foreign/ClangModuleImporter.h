#ifndef STONE_CLANG_CLANGMODULEIMPORTER_H
#define STONE_CLANG_CLANGMODULEIMPORTER_H

#include "stone/Foreign/ClangModuleLoader.h"


#include "llvm/ADT/ArrayRef.h"

namespace stone {

class ClangModuleImporter final : public ClangModuleLoader {
public:
  ClangModuleImporter();
};

} // namespace stone
#endif
