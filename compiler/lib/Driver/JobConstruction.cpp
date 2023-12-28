#include "stone/Driver/JobConstruction.h"

using namespace stone;

// CompileJobConstruction::CompileJobConstruction(file::Type outputFileType)
//     : JobConstruction(JobConstructionKind::Compile, nullptr, outputFileType) {}

CompileJobConstruction::CompileJobConstruction(
    JobConstructionInput primaryInput, file::Type outputFileType)
    : JobConstruction(JobConstructionKind::Compile, primaryInput,
                      outputFileType) {}
