#ifndef STONE_CORE_PRINTABLE_H
#define STONE_CORE_PRINTABLE_H

#include "stone/Core/Color.h"
#include "llvm/Support/raw_ostream.h"

namespace stone {

class PrintingPolicy {
public:
};

class Printable {
public:
  virtual ~Printable() {}

public:
  virtual void Print(ColorOutputStream &os,
                     const PrintingPolicy *policy = nullptr) const = 0;
};

} // namespace stone
#endif
