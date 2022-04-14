#ifndef STONE_COMPILE_STCOPTIONS_H
#define STONE_COMPILE_STCOPTIONS_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/SystemOptions.h"
#include "stone/Compile/TargetOptions.h"
#include "stone/Compile/TypeCheckerOptions.h"
#include "stone/Session/BaseOptions.h"
#include "stone/Session/Options.h"
#include "stone/Syntax/SearchPathOptions.h"

using namespace stone::types;

namespace stone {

enum class ModuleOutputMode : uint8_t {
  None = 0,
  Single,
  Whole,
};

enum class ThreadModelKind {
  /// POSIX Threads.
  POSIX,
  /// Single Threaded Environment.
  Single
};

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

  TargetOptions targetOpts;

  ModuleOutputMode moduleOutputMode = ModuleOutputMode::None;

  ThreadModelKind threadModelKind = ThreadModelKind::POSIX;

public:
  LangOptions() {
    /// The default mode kind of the system
    defaultModeKind = ModeKind::EmitObject;
  }
};

} // namespace stone
#endif
