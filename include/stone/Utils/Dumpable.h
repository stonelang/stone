#ifndef STONE_UTILS_DUMPABLE_H
#define STONE_UTILS_DUMPABLE_H

namespace stone {
class Dumpable {
public:
  virtual ~Dumpable() {}

public:
  virtual void Dump() const = 0;
};
} // namespace stone
#endif
