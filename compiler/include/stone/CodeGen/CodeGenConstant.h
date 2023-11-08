#ifndef STONE_GEN_IRCODEGENCONST_H
#define STONE_GEN_IRCODEGENCONST_H

#include "stone/CodeGen/CodeGenFunction.h"
#include "stone/CodeGen/CodeGenModule.h"

namespace stone {

class CodeGenConstant final {
public:
  CodeGenModule &cgm;
  CodeGenFunction *cgf;

public:
  CodeGenConstant(const CodeGenConstant &other) = delete;
  CodeGenConstant &operator=(const CodeGenConstant &other) = delete;

public:
  CodeGenConstant(CodeGenModule &cgm, CodeGenFunction *cgf = nullptr);

  /// Initialize this emission in the context of the given function.
  /// Use this if the expression might contain contextual references like
  /// block addresses or PredefinedExprs.
  CodeGenConstant(CodeGenFunction &cgf);

  ~CodeGenConstant();
};
} // namespace stone
#endif
