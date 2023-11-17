#ifndef STONE_COMPILE_COMPILEROPTIONS_H
#define STONE_COMPILE_COMPILEROPTIONS_H

#include "stone/Option/ActionKind.h"

namespace stone {

class CompilerOptionsConverter;
class CompilerInputsConverter;

class CompilerOptions final {

  friend CompilerInvocation;
  friend CompilerOptionsConverter;
  friend CompilerInputsConverter;

public:

  /// The action request
  ActionKind MainActionKind;

  /// The name of the action 
  llvm::StringRef MainActionName;

  /// A list of arbitrary modules to import and make implicitly visible.
  std::vector<std::pair<String, bool /*testable*/>> ImplicitModuleNames;
};

#endif