#include "stone/Driver/Compilation.h"

using namespace stone;

Compilation::Compilation(CompilationKind kind, Driver &driver)
    : kind(kind), driver(driver) {}

QuadraticCompilation::QuadraticCompilation(Driver &driver)
    : Compilation(CompilationKind::Quadratic, driver) {}

Status QuadraticCompilation::Execute() { return Status(); }

FlatCompilation::FlatCompilation(Driver &driver)
    : Compilation(CompilationKind::Flat, driver) {}

Status FlatCompilation::Execute() { return Status(); }

CPUCountCompilation::CPUCountCompilation(Driver &driver)
    : Compilation(CompilationKind::CPUCount, driver) {}

Status CPUCountCompilation::Execute() { return Status(); }

SingleCompilation::SingleCompilation(Driver &driver)
    : Compilation(CompilationKind::Single, driver) {}

Status SingleCompilation::Execute() { return Status(); }
