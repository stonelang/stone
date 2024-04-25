#include "stone/Syntax/Attribute.h"

StringRef stone::getAccessLevelSpelling(AccessLevel value) {
  switch (value) {
  case AccessLevel::Private:
    return "private";
  case AccessLevel::Internal:
    return "internal";
  case AccessLevel::Public:
    return "public";
  case AccessLevel::Global:
    return "global";
  case AccessLevel::File:
    return "global";
  }

  llvm_unreachable("Unhandled AccessLevel in switch.");
}
