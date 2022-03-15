#ifndef STONE_GEN_IRBLOCKS_H
#define STONE_GEN_IRBLOCKS_H

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class IRBlocks {
public:
  IRBlocks();
};

} // namespace stone
#endif
