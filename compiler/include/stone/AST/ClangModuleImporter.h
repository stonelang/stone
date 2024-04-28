#ifndef STONE_SYNTAX_CLANGMODULEIMPORTER_H
#define STONE_SYNTAX_CLANGMODULEIMPORTER_H

#include "stone/AST/ClangModuleLoader.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class ClangModuleImporterOptions final {};
class ClangModuleImporter : public ClangModuleLoader {

  ClangModuleImporterOptions importerOptions;

public:
  ClangModuleImporter();

public:
  ClangModuleImporterOptions &GetClangModuleImporterOptions() {
    return importerOptions;
  }
};

} // namespace stone
#endif