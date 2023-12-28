#include "stone/Driver/Compilation.h"

QuadraticCompilation::QuadraticCompilation()
    : Compilation(CompilationKind::Quadratic) {}

FlatCompilation::FlatCompilation() : Compilation(CompilationKind::Flat) {}

CPUCountCompilation::CPUCountCompilation()
    : Compilation(CompilationKind::CPUCount) {}

SingleCompilation::SingleCompilation() : Compilation(CompilationKind::Single) {}
