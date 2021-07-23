#ifndef STONE_CODEGEN_CODEGENBLOCKS_H
#define STONE_CODEGEN_CODEGENBLOCKS_H

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {
namespace codegen {

class CodeGenBlocks {
public:
  CodeGenBlocks();
};

} // namespace codegen
} // namespace stone
#endif
