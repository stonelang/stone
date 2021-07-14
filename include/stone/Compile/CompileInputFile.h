#ifndef STONE_COMPILE_COMPILEINPUTFILE_H
#define STONE_COMPILE_COMPILEINPUTFILE_H

#include "stone/Compile/CompilableItem.h"
#include "llvm/ADT/ArrayRef.h"

#include <memory>

namespace stone {
class Compiler;
std::unique_ptr<CompilableItem> CompileInputFile(Compiler &compiler,
                                                 file::File &input);

} // namespace stone
#endif
