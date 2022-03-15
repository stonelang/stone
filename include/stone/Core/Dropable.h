#ifndef STONE_CORE_DROPABLE_H
#define STONE_CORE_DROPABLE_H

namespace stone {
class Dropable {
public:
  virtual ~Dropable();

public:
  virtual void Drop() = 0;
};
} // namespace stone
#endif
