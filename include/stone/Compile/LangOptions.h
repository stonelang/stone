#ifndef STONE_COMPILE_STCOPTIONS_H
#define STONE_COMPILE_STCOPTIONS_H

#include "stone/Compile/TypeCheckerOptions.h"
#include "stone/Core/FileSystemOptions.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Core/SystemOptions.h"
#include "stone/Gen/CodeGenOptions.h"
#include "stone/Session/BaseOptions.h"
#include "stone/Session/Options.h"
#include "stone/Syntax/SearchPathOptions.h"

using namespace stone::types;

namespace stone {

class LangOptions final : public BaseOptions {
public:
  /// Options for the entire system
  SystemOptions systemOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

public:
  LangOptions() {
    /// The default mode kind of the system
    defaultModeKind = ModeKind::EmitObject;
  }
};

} // namespace stone
#endif
