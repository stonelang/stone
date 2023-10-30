#include "stone/AST/ClangContext.h"

using namespace stone;

ClangContext::ClangContext() : clangInstance(new clang::CompilerInstance()) {}