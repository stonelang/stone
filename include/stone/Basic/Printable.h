#ifndef STONE_BASIC_PRINTABLE_H
#define STONE_BASIC_PRINTABLE_H

#include "llvm/Support/raw_ostream.h"

namespace stone {

class PrintingPolicy {
public:
};

class Printable {
public:
  virtual ~Printable() {}

public:
  virtual void Print(llvm::raw_ostream &os,
                     const PrintingPolicy &policy) const = 0;
};

} // namespace stone
#endif
