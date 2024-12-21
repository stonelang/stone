#ifndef STONE_DIAG_DIAGNOSTIC_EMITTER_H
#define STONE_DIAG_DIAGNOSTIC_EMITTER_H

#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Diag/DiagnosticClient.h"

#include "llvm/ADT/PointerUnion.h"

namespace stone {
class SrcLoc;
class LangOptions;
class DiagnosticOptions;

namespace diags {

class TopLevelDiagnostic;
class StoredDiagnostic;

class PresumedLoc {
  //   unsigned BufferID;
  //   unsigned Line, Col;

  // public:
  //   PresumedLoc() = default;
  //   PresumedLoc(unsigned BufferID, unsigned Line, unsigned Col)
  //       : Filename(FN), BufferID(BufferID), Line(Line), Col(Col) {}

  // public:
  //   bool IsValid() const { return (BufferID > 0); }

  //   unsigned GetBufferID() const {
  //     assert(IsValid());
  //     return BufferID;
  //   }

  //   /// Return the presumed line number of this location.
  //   ///
  //   /// This can be affected by \#line etc.
  //   unsigned GetLine() const {
  //     assert(IsValid());
  //     return Line;
  //   }

  //   /// Return the presumed column number of this location.
  //   ///
  //   /// This cannot be affected by \#line, but is packaged here for
  //   convenience. unsigned GetColumn() const {
  //     assert(IsValid());
  //     return Col;
  //   }
};

class FullSrcLoc : public SrcLoc {
  // const SrcMgr& SM;

  // public:
  // /// Creates a FullSourceLoc where isValid() returns \c false.
  // FullSourceLoc() = default;

  // explicit FullSourceLoc(SrcLoc Loc, const SrcMgr &SM)
  //     : SrcLoc(Loc), SrcMgr(&SM) {}

  // const SrcMgr &GetSrcMgr() const {
  //   return SM;
  // }

  // unsigned GetBufferID() const;
};

using TopLevelDiagnosticOrStoredDiagnotic =
    llvm::PointerUnion<const TopLevelDiagnostic *, const StoredDiagnostic *>;

/// DiagnosticRenderer in clang
class DiagnosticEmitter {
protected:
  DiagnosticOutputStream &OS;

  const LangOptions &LangOpts;

  const DiagnosticOptions &DiagOpts;

  /// The location of the previous diagnostic if known.
  ///
  /// This will be invalid in cases where there is no (known) previous
  /// diagnostic location, or that location itself is invalid or comes from
  /// a different source manager than SM.
  SrcLoc LastLoc;

  /// The level of the last diagnostic emitted.
  ///
  /// The level of the last diagnostic emitted. Used to detect level changes
  /// which change the amount of information displayed.
  DiagnosticLevel LastLevel = DiagnosticLevel::Ignore;

protected:
  DiagnosticEmitter(DiagnosticOutputStream &OS, const LangOptions &LangOpts,
                    const DiagnosticOptions &DiagOpts);
  virtual ~DiagnosticEmitter();

protected:
  virtual void EmitDiagnosticMessage(FullSrcLoc Loc, PresumedLoc PLoc,
                                     DiagnosticLevel Level,
                                     llvm::StringRef Message,
                                     ArrayRef<CharSrcRange> Ranges,
                                     TopLevelDiagnosticOrStoredDiagnotic Info) = 0;

  virtual void EmitDiagnosticLoc(FullSrcLoc Loc, PresumedLoc PLoc,
                                 DiagnosticLevel Level,
                                 ArrayRef<CharSrcRange> Ranges) = 0;

  virtual void BeginDiagnostic(TopLevelDiagnosticOrStoredDiagnotic D,
                               DiagnosticLevel Level) {}

  virtual void EndDiagnostic(TopLevelDiagnosticOrStoredDiagnotic D,
                             DiagnosticLevel Level) {}

  virtual void EmitCodeContext(FullSrcLoc Loc, DiagnosticLevel Level,
                               llvm::SmallVectorImpl<CharSrcRange> &Ranges,
                               llvm::ArrayRef<FixIt> FixIts) = 0;

public:
  DiagnosticOutputStream &GetDiagnosticOutputStream() { return OS; }
  const LangOptions &GetLangOptions() const { return LangOpts; }
  const DiagnosticOptions &GetDiagOptions() const { return DiagOpts; }
};

} // namespace diags

} // namespace stone

#endif