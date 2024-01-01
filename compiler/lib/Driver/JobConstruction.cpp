#include "stone/Driver/JobConstruction.h"
#include "stone/Basic/File.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverPrettyStackTrace.h"

using namespace stone;
using namespace stone::file;

CompileJobConstruction::CompileJobConstruction(FileType outputFileType)
    : IncrementatlJobConstruction(CompilationEntityKind::CompileJobConstruction,
                                  llvm::None, outputFileType) {

  assert(file::IsOutputableFileType(outputFileType));
}

CompileJobConstruction::CompileJobConstruction(const CompilationEntity *input,
                                               FileType outputFileType)
    : IncrementatlJobConstruction(CompilationEntityKind::CompileJobConstruction,
                                  input, outputFileType) {

  assert(file::IsOutputableFileType(outputFileType));
}
