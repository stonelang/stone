#ifndef STONE_AST_SUBSTITUTION_H
#define STONE_AST_SUBSTITUTION_H

#include "llvm/ADT/PointerUnion.h"

namespace stone {

/// Flags that can be passed when substituting into a type.
// enum class SubstitutionFlags {
//   /// Allow substitutions to recurse into SILFunctionTypes.
//   /// Normally, SILType::subst() should be used for lowered
//   /// types, however in special cases where the substitution
//   /// is just changing between contextual and interface type
//   /// representations, using Type::subst() is allowed.
//   AllowLoweredTypes = 0x01,
//   /// Map member types to their desugared witness type.
//   DesugarMemberTypes = 0x02,
//   /// Substitute types involving opaque type archetypes.
//   SubstituteOpaqueArchetypes = 0x04
// };

/// Options for performing substitutions into a type.
// struct SubstitutionOptions : public llvm::OptionSet<SubstitutionFlags> {
//   // Note: The unfortunate use of Type * here, rather than Type,
//   // is due to a libc++ quirk that requires the result type to be
//   // complete.
//   typedef std::function<Type *(const NormalInterfaceConformance *,
//                                    AssociatedTypeDecl *)>
//     GetTentativeTypeWitness;

//   /// Function that retrieves a tentative type witness for a protocol
//   /// conformance with the state \c CheckingTypeWitnesses.
//   GetTentativeTypeWitness getTentativeTypeWitness;

//   SubstitutionOptions(std::nullopt_t) : OptionSet(None) { }

//   SubstitutionOptions(SubstFlags flags) : OptionSet(flags) { }

//   SubstitutionOptions(llvm::OptionSet<SubstitutionFlags> options) :
//   llvm::OptionSet(options) { }
// };

// inline SubstOptions operator|(SubstFlags lhs, SubstFlags rhs) {
//   return SubstOptions(lhs) | rhs;
// }
class SubstitutionMap {
public:
};

} // namespace stone

#endif
