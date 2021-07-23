#ifndef STONE_COMPILE_COMPILEOPTIONS_H
#define STONE_COMPILE_COMPILEOPTIONS_H

#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/CodeGen/CodeGenOptions.h"
#include "stone/Session/SessionOptions.h"
#include "stone/Syntax/SearchPathOptions.h"

using namespace stone::codegen;

namespace stone {

class CompilerOptions final : public SessionOptions {
public:
  CodeGenOptions genOpts;
  SearchPathOptions spOpts;
  bool typeCheckwholeModule = false;

public:
  CompilerOptions() {}
};

} // namespace stone
#endif
