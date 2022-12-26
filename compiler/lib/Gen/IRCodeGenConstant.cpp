#include "stone/Gen/IRCodeGenConstant.h"

using namespace stone;

IRCodeGenConstant::IRCodeGenConstant(IRCodeGenModule &cgm,
                                     IRCodeGenFunction *cgf) : cgm(cgm), cgf(cgf) {}

/// Initialize this emission in the context of the given function.
/// Use this if the expression might contain contextual references like
/// block addresses or PredefinedExprs.
IRCodeGenConstant::IRCodeGenConstant(IRCodeGenFunction &cgf)
    : cgm(cgf.GetIRCodeGenModule()), cgf(&cgf) {}

IRCodeGenConstant::~IRCodeGenConstant() {}
