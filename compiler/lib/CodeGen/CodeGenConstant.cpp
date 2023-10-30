#include "stone/CodeGen/CodeGenConstant.h"

using namespace stone;

CodeGenConstant::CodeGenConstant(CodeGenModule &cgm,
                                     CodeGenFunction *cgf)
    : cgm(cgm), cgf(cgf) {}

/// Initialize this emission in the context of the given function.
/// Use this if the expression might contain contextual references like
/// block addresses or PredefinedExprs.
CodeGenConstant::CodeGenConstant(CodeGenFunction &cgf)
    : cgm(cgf.GetCodeGenModule()), cgf(&cgf) {}

CodeGenConstant::~CodeGenConstant() {}
