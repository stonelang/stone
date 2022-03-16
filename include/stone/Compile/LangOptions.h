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

  /// The default mode kind of the system
  ModeKind defaultModeKind = ModeKind::EmitObject;

  // private:
  //   OptUtil optUtil;
  // public:
  //   OptUtil& GetOptUtil() { return optUtil; }
};

} // namespace stone
#endif
