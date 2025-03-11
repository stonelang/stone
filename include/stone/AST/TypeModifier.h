#ifndef STONE_AST_TYPEMODIFIER_H
#define STONE_AST_TYPEMODIFIER_H

class TypeModifierFlags final {
public:
  enum ID : unsigned {
    None = 0,
    Const = 1 << 0,
    Pure = 1 << 1,
    Restrict = 1 << 2,
    Volatile = 1 << 3,
    Stone = 1 << 4,
  };

private:
  unsigned flags = ID::None;

public:
  // Set specific modifiers using bitwise OR
  void AddModifier(ID modifier) { flags |= modifier; }

  // Check for specific modifiers
  bool HasConst() const { return flags & ID::Const; }
  bool HasPure() const { return flags & ID::Pure; }
  bool HasRestrict() const { return flags & ID::Restrict; }
  bool HasVolatile() const { return flags & ID::Volatile; }
  bool HasStone() const { return flags & ID::Stone; }

  // Remove a modifier if needed
  void RemoveModifier(ID modifier) { flags &= ~modifier; }

  // Clear all modifiers
  void Clear() { flags = ID::None; }
};

#endif