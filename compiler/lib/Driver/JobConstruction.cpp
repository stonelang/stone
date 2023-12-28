#include "stone/Driver/JobConstruction.h"

using namespace stone;

CompileJobConstruction::CompileJobConstruction(file::Type outputFileType)
    : JobConstruction(JobConstructionKind::Compile, llvm::None,
                      outputFileType) {}

CompileJobConstruction::CompileJobConstruction(JobConstructionInput input,
                                               file::Type outputFileType)
    : JobConstruction(JobConstructionKind::Compile, input, outputFileType) {}
