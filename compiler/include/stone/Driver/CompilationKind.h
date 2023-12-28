#ifndef STONE_DRIVER_DRIVER_COMPILATION_KIND_H
#define STONE_DRIVER_DRIVER_COMPILATION_KIND_H

namespace stone {
/// This mode controls the compilation process
/// p := -primary-file
enum class CompilationKind : uint8_t {
  /// n inputs, n compile(s), n * n  parses
  /// Ex: compile_1(1=p ,...,n), compile_2(1,2=p,...,n),...,
  /// compile_n(1,....,n=p)
  Quadratic = 0,
  /// n input(s), n compile(s), n parses
  /// Ex: compile_1(1=p), compile_2(2=p),..., compile_n(n=p)
  Flat,
  /// n inputs, j CPU(s), j compile(s), n * j parses
  /// Ex: compile_1(1=p,...,n),...,
  /// compile_2(1,2=p,...,n),...,compile_j(1,...,p=j,...,n)
  CPUCount,
  /// n inputs, 1 compile, n parses
  /// Ex: compile(1,....,n)
  Single,
};
} // namespace stone

#endif