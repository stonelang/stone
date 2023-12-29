#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

Compilation::Compilation(const Driver &driver) : driver(driver) {}

// CompilationKind Compilation::GetKind() const {
//   return driver.GetInvocation().GetCompilationKind();
// }

// Compilation::~Compilation() {}

// QuadraticCompilation::QuadraticCompilation(Driver &driver)
//     : Compilation(driver) {}

// QuadraticCompilation::BuildTopLevelJobConstructions() {}

// Status QuadraticCompilation::Execute() { return Status(); }

// FlatCompilation::FlatCompilation(Driver &driver) : Compilation(driver) {}

// Status FlatCompilation::Execute() { return Status(); }

// CPUCountCompilation::CPUCountCompilation(Driver &driver)
//     : Compilation(driver) {}

// Status CPUCountCompilation::Execute() { return Status(); }

// SingleCompilation::SingleCompilation(Driver &driver) : Compilation(driver) {}

// Status SingleCompilation::Execute() { return Status(); }

Status Driver::ExecuteCompilation() {}
