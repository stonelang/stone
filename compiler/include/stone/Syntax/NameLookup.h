#ifndef STONE_AST_NAMELOOKUP_H
#define STONE_AST_NAMELOOKUP_H

namespace stone {
namespace ast {
/// This is a specifier for the kind of name lookup being performed
/// by various query methods.
enum class NameLookupKind { None = 0, Unqualified, Qualified };

class NameLookup final {
public:
};

} // namespace ast
} // namespace stone
#endif
