#ifndef STONE_GEN_IRCODEGENCONST_H
#define STONE_GEN_IRCODEGENCONST_H

#include "stone/Gen/IRGenFunction.h"
#include "stone/Gen/IRGenInvocation.h"

namespace stone {

class IRGenConstant final {
public:
  IRGenModule &cgm;
  IRGenFunction *cgf;

public:
  IRGenConstant(const IRGenConstant &other) = delete;
  IRGenConstant &operator=(const IRGenConstant &other) = delete;

public:
  IRGenConstant(IRGenModule &cgm, IRGenFunction *cgf = nullptr);

  /// Initialize this emission in the context of the given function.
  /// Use this if the expression might contain contextual references like
  /// block addresses or PredefinedExprs.
  IRGenConstant(IRGenFunction &cgf);

  ~IRGenConstant();
};

} // namespace stone
#endif
