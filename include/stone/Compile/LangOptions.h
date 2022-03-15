#ifndef STONE_COMPILE_STCOPTIONS_H
#define STONE_COMPILE_STCOPTIONS_H

#include "stone/Compile/TypeCheckerOptions.h"
#include "stone/Core/FileSystemOptions.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Core/SystemOptions.h"
#include "stone/Gen/CodeGenOptions.h"
#include "stone/Option/Options.h"
#include "stone/Syntax/SearchPathOptions.h"

using namespace stone::types;

namespace stone {

class LangOptions final {
public:
  /// Options for the entire system
  SystemOptions systemOpts;
  /// Options for generating code
  CodeGenOptions codeGenOpts;
  /// The options for searching libs
  SearchPathOptions searchPathOpts;
  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  /// When performing type-checking, we can shoose to type-check
  /// each syn::SyntaxFile or wait to type-check the entire syn::Module contains
  /// syn::SyntaxFile instances
  bool typeCheckWholeModule = false;

  /// The name of the primary module - you will get this from parsing
  llvm::StringRef moduleName;

  /// The default mode kind of the system
  ModeKind defaultModeKind = ModeKind::EmitObject;
};

} // namespace stone
#endif
