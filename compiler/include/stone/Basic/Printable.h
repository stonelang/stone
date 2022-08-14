#ifndef STONE_BASIC_PRINTABLE_H
#define STONE_BASIC_PRINTABLE_H

#include "stone/Basic/Color.h"
#include "llvm/Support/raw_ostream.h"

namespace stone {

class PrintingPolicy {
public:
};

class Printable {
public:
  virtual ~Printable() {}

public:
  virtual void Print(ColorfulStream &os,
                     const PrintingPolicy *policy = nullptr) const = 0;
};

} // namespace stone
#endif
