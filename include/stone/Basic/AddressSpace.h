#ifndef STONE_BASIC_ADDRESSPACE_H
#define STONE_BASIC_ADDRESSPACE_H

#include <cassert>
// NOTE: You may not use ths since it is opencl stuff ... I think.
namespace stone {

/// Defines the address space values used by the address space qualifier
/// of QualType.
///
enum class AddressSpaceKind : unsigned {
  // The default value 0 is the value used in QualType for the situation
  // where there is no address space qualifier.
  Default = 0,

  // Pointer size and extension address spaces.
  SPtr32,
  UPtr32,
  Ptr64,
  // This denotes the count of language-specific address spaces and also
  // the offset added to the target-specific address spaces, which are usually
  // specified by address space attributes __attribute__(address_space(n))).
  FirstTarget
};

struct AddressSpace final {
  /// The type of a lookup table which maps from language-specific address
  /// spaces
  /// to target-specific ones.
  using AddressSpaceMap = unsigned[(unsigned)AddressSpaceKind::FirstTarget];

  /// \return whether \p AS is a target-specific address space rather than a
  /// clang AST address space
  static bool IsTargetAddressSpace(AddressSpaceKind kind) {
    return (unsigned)kind >= (unsigned)AddressSpaceKind::FirstTarget;
  }

  static unsigned ToTargetAddressSpace(AddressSpaceKind kind) {
    assert(IsTargetAddressSpace(kind));
    return (unsigned)kind - (unsigned)AddressSpaceKind::FirstTarget;
  }

  static AddressSpaceKind GetByAddressSpace(unsigned targetAddressSpace) {
    return static_cast<AddressSpaceKind>(
        (targetAddressSpace) + (unsigned)AddressSpaceKind::FirstTarget);
  }

  static bool IsPtrSizeAddressSpace(AddressSpaceKind kind) {
    return (kind == AddressSpaceKind::SPtr32 ||
            kind == AddressSpaceKind::UPtr32 ||
            kind == AddressSpaceKind::Ptr64);
  }
};

} // namespace stone
#endif