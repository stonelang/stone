#include "stone/Gen/IRGenConstant.h"

using namespace stone;

IRGenConstant::IRGenConstant(IRGenModule &cgm, IRGenFunction *cgf)
    : cgm(cgm), cgf(cgf) {}

/// Initialize this emission in the context of the given function.
/// Use this if the expression might contain contextual references like
/// block addresses or PredefinedExprs.
IRGenConstant::IRGenConstant(IRGenFunction &cgf)
    : cgm(cgf.GetIRGenModule()), cgf(&cgf) {}

IRGenConstant::~IRGenConstant() {}
