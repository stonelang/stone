#include "clang/Basic/CharInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ConvertUTF.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Unicode.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <queue>
#include <string>
#include <utility>
#include <vector>

class TextToken {
  enum Kind : unsigned {
#define TOKEN(X) X,
#include "stone/Basic/TokenKind.def"
    TotalTokTypes
  };
};
struct TextSlice {};

class TextLexer {
public:
};

class TextParser {
  llvm::SmallVector<TextSlice> slices;

public:
  void AddSlice(TextSlice slice) { slices.push_back(slice); }
  void Parse() {}

public:
  TextParser(llvm::StringRef Intext);
};

class TextFormatter {
public:
  void Format(llvm::ArrayRef<TextSlice> slices) {}
};
