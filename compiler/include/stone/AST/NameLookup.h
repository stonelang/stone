#ifndef STONE_ASTNAMELOOKUP_H
#define STONE_ASTNAMELOOKUP_H

namespace stone {

/// This is a specifier for the kind of name lookup being performed
/// by various query methods.
enum class NameLookupKind { None = 0, Unqualified, Qualified };

class NameLookup final {
public:
};

} // namespace stone
#endif
