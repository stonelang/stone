#ifndef STONE_AST_FUNCTION_SPECIFIER_H
#define STONE_AST_FUNCTION_SPECIFIER_H

#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/SmallVector.h"

namespace stone {

enum class FunctionInlineSpecifierKind : UInt8 {
  None = 0,
  Inline,
  ForcedInline,
};

enum class FunctionCallingConvention : UInt8 { Stone, C };

class FunctionSpecifierCollector final {

  unsigned flags;
  SrcLoc inlineLoc;
  SrcLoc forcedInlineLoc;
  SrcLoc virtualLoc;
  SrcLoc funLoc;
  SrcLoc arrowLoc;
  SrcLoc doubleColonLoc;

  enum Flags : unsigned {
    None = 1 << 0,
    Fun = 1 << 1,
    Inline = 1 << 2,
    ForcedInline = 1 << 3,
    Virtual = 1 << 4,
    NoReturn = 1 << 5,
    IsMember = 1 << 6,
    IsStatic = 1 << 7,
  };

public:
  FunctionSpecifierCollector()
      : flags(0), inlineLoc(SrcLoc()), forcedInlineLoc(SrcLoc()),
        virtualLoc(SrcLoc()), funLoc(SrcLoc()), doubleColonLoc(SrcLoc()) {}

public:
  void AddFun(SrcLoc loc) {
    flags |= Fun;
    funLoc = loc;
  }
  bool HasFun() { return ((flags & Fun) && funLoc.isValid()); }
  SrcLoc GetFunLoc() { return funLoc; }

  void AddInline(SrcLoc loc) {
    flags |= Inline;
    inlineLoc = loc;
  }
  bool HasInline() { return ((flags & Inline) && inlineLoc.isValid()); }
  SrcLoc GetInlineLoc() { return inlineLoc; }

  void AddForcedInline(SrcLoc loc) {
    flags |= ForcedInline;
    forcedInlineLoc = loc;
  }
  bool HasForcedInline() {
    return ((flags & ForcedInline) && forcedInlineLoc.isValid());
  }

  void AddVirtual(SrcLoc loc) { flags |= Virtual; }
  bool HasVirtual() { return ((flags & Virtual) && virtualLoc.isValid()); }

  void AddNoReturn(SrcLoc loc) { flags |= NoReturn; }
  bool HasNoReturn() { return (flags & NoReturn); }

  void AddArrowLoc(SrcLoc loc) { arrowLoc = loc; }
  SrcLoc GetArrowLoc() { return arrowLoc; }
  bool HasArrow() { return GetArrowLoc().isValid(); }

  void AddIsMember(SrcLoc inputLoc) {
    flags |= IsMember;
    doubleColonLoc = inputLoc;
  }
  bool HasIsMember() { return (flags & IsMember) && doubleColonLoc.isValid(); }
  SrcLoc GetDoubleColonLoc() { return doubleColonLoc; }

public:
  void Apply();

  void ClearAll() {
    flags = 0;
    inlineLoc = SrcLoc();
    forcedInlineLoc = SrcLoc();
    virtualLoc = SrcLoc();
    funLoc = SrcLoc();
    arrowLoc = SrcLoc();
    doubleColonLoc = SrcLoc();
  }
};

} // namespace stone
#endif
