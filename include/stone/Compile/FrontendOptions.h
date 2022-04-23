#ifndef STONE_COMPILE_LANGOPTIONS_H
#define STONE_COMPILE_LANGOPTIONS_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Compile/FrontendInputsAndOutputs.h"
#include "stone/Compile/TargetOptions.h"
#include "stone/Sem/TypeCheckerOptions.h"
#include "stone/Session/BaseOptions.h"
#include "stone/Session/Options.h"
#include "stone/Syntax/SearchPathOptions.h"

using namespace stone::sem;

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

class FrontendOptions final : public BaseOptions {
  friend class Frontend;
  /// A list of arbitrary modules to import and make implicitly visible.
  std::vector<std::pair<std::string, bool /*testable*/>> implicitModuleNames;

public:
  ModuleOutputMode moduleOutputMode = ModuleOutputMode::None;

  ThreadModelKind threadModelKind = ThreadModelKind::POSIX;

  FrontendInputsAndOutputs inputsAndOutputs;

public:
  //== Proprties == /

  /// Indicates that the input(s) should be parsed as the Stone stdlib.
  bool shouldParseAsStdLib = false;

public:
  FrontendOptions(const Mode &mode);

public:
  static file::Type GetFileTypeByModeKind(ModeKind modeKind);
};

} // namespace stone
#endif
