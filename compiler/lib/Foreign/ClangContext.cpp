#include "stone/Foreign/ClangContext.h"

using namespace stone;

ClangContext::ClangContext() : clangInstance(new clang::CompilerInstance()) {}