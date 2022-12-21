#include "stone/Foreign/Clang.h"

using namespace stone;

Clang::Clang()
    : compilerInstance(new clang::CompilerInstance()),
      clangModuleImporter(new ClangModuleImporter()) {}