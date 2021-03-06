#ifndef STONE_BASIC_DUMPABLE_H
#define STONE_BASIC_DUMPABLE_H

namespace stone {
class Dumpable {
public:
  virtual ~Dumpable() {}

public:
  virtual void Dump() const = 0;
};
} // namespace stone
#endif
