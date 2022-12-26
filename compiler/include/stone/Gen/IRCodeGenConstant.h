#ifndef STONE_GEN_IRCODEGENCONST_H
#define STONE_GEN_IRCODEGENCONST_H

#include "stone/Gen/IRCodeGenFunction.h"
#include "stone/Gen/IRCodeGenModule.h"

namespace stone {

class IRCodeGenConstant final {
public:
  IRCodeGenModule &cgm;
  IRCodeGenFunction *cgF;

public:
  IRCodeGenConstant(const IRCodeGenConstant &other) = delete;
  IRCodeGenConstant &operator=(const IRCodeGenConstant &other) = delete;

public:
  IRCodeGenConstant(IRCodeGenModule &cgm, IRCodeGenFunction *cgf = nullptr)
      : cgm(cgm), cgf(cgf) {}

  /// Initialize this emission in the context of the given function.
  /// Use this if the expression might contain contextual references like
  /// block addresses or PredefinedExprs.
  IRCodeGenConstant(IRCodeGenFunction &cgf)
      : cgm(cgf.GetIRCodeGenModule()), cgf(&cgf) {}

  ~IRCodeGenConstant();
};

} // namespace stone
#endif
