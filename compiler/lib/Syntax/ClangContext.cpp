#include "stone/Syntax/ClangContext.h"

using namespace stone;

ClangContext::ClangContext() : clangInstance(new clang::CompilerInstance()) {}