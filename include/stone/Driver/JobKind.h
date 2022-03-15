#ifndef STONE_COMPILER_JOBKIND_H
#define STONE_COMPILER_JOBKIND_H

namespace stone {
enum class JobKind {
  None,
  Compile,
  Backend,
  MergeModule,
  DynamicLink,
  StaticLink,
  ExecutableLink,
  Assemble
};
} // namespace stone
#endif
