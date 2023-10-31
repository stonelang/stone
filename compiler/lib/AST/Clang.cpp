#include "stone/AST/Clang.h"

using namespace stone;

Clang::Clang() : clangInstance(new clang::CompilerInstance()) {}