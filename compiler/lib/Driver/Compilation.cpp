#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

Compilation::Compilation(CompilationKind kind, const Driver &driver)
    : kind(kind), driver(driver) {}

Compilation::~Compilation() {}

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

Status Driver::ExecuteCompilation() {}
