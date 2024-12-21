
#include "stone/Diag/DiagnosticBasicKind.h"
using namespace stone;

enum class diags::DiagID : uint32_t {
#define DIAG(KIND, ID, Options, Message, Signature) ID,
#include "stone/Diag/DiagnosticEngine.def"
};
static_assert(static_cast<uint32_t>(stone::diags::DiagID::invalid_diagnostic) ==
                  0,
              "0 is not the invalid diagnostic ID");

// Define all of the diagnostic objects and initialize them with their
// diagnostic IDs.
namespace stone {
namespace diags {
#define DIAG(KIND, ID, Options, Message, Signature)                            \
  DiagWithArguments<void Signature>::type ID = {DiagID::ID};

#include "stone/Diag/DiagnosticEngine.def"
} // namespace diags
} // end namespace stone

// enum class diags::DiagID : uint32_t {
// #define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR, \
//              SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
//   ENUM,
// #include "stone/Diag/AllDiagnosticKind.inc"
// };

// namespace stone {
// namespace diags {
// #define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR, \
//              SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
//   Diag::ID ENUM = {DiagID::ENUM};
// #include "stone/Diag/AllDiagnosticKind.inc"
// } // namespace diags
// } // end namespace stone

// namespace {
// // Get an official count of all of the diagnostics in the system
// enum LocalDiagID : uint32_t {
// #define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR, \
//              SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
//   ENUM,
// #include "stone/Diag/AllDiagnosticKind.inc"
//   TotalDiags
// };
// } // namespace

// namespace {

// static constexpr const char *const DiagnosticStrings[] = {
// #define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR, \
//              SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
//   DESC,
// #include "stone/Diag/AllDiagnosticKind.inc"
//     "<not a diagnostic>",
// };

// static constexpr const char *const DiagnosticIDStrings[] = {
// #define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR, \
//              SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
//   #ENUM,
// #include "stone/Diag/AllDiagnosticKind.inc"
//     "<not a diagnostic>",
// };

// // static const constexpr StoredDiagnosticInfo
// StoredDiagnosticInfos[] = {
// // #define ERROR(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE,
// NOWERROR,SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
// //   StoredDiagnosticInfo(DiagnosticKind::Error,
// LocalDiagnosticOptions::Options),
// // #define WARNING(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE,
// NOWERROR,SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
// //   StoredDiagnosticInfo(DiagnosticKind::Warning, \
// //                        LocalDiagnosticOptions::Options),
// // #define NOTE(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE,
// NOWERROR,SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
// //   StoredDiagnosticInfo(DiagnosticKind::Note,
// LocalDiagnosticOptions::Options),
// // #define REMARK(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE,
// NOWERROR,SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
// //   StoredDiagnosticInfo(DiagnosticKind::Remark,
// LocalDiagnosticOptions::Options),
// // #include "stone/Diag/AllDiagnosticKind.inc"
// // };
// // static_assert(sizeof(storedDiagnosticInfos) /
// sizeof(StoredDiagnosticInfo)
// ==
// //                   LocalDiagID::TotalDiags,
// //               "array size mismatch");

// struct StaticDiagInfoRec;

// // Store the descriptions in a separate table to avoid pointers that need to
// // be relocated, and also decrease the amount of data needed on 64-bit
// // platforms. See "How To Write Shared Libraries" by Ulrich Drepper.
// struct StaticDiagInfoDescriptionStringTable {
// #define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR, \
//              SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
//   char ENUM##_desc[sizeof(DESC)];
//   // clang-format off
// #include "stone/Diag/DiagnosticBasicKind.inc"
// #include "stone/Diag/DiagnosticParseKind.inc"

//   // clang-format on
// #undef DIAG
// };

// const StaticDiagInfoDescriptionStringTable StaticDiagInfoDescriptions = {
// #define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR, \
//              SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
//   DESC,
// // clang-format off
// #include "stone/Diag/DiagnosticBasicKind.inc"
// #include "stone/Diag/DiagnosticParseKind.inc"
// // clang-format on
// #undef DIAG
// };

// extern const StaticDiagInfoRec StaticDiagInfo[];
// // Stored separately from StaticDiagInfoRec to pack better.  Otherwise,
// // StaticDiagInfoRec would have extra padding on 64-bit platforms.
// const uint32_t StaticDiagInfoDescriptionOffsets[] = {
// #define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR, \
//              SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
//   offsetof(StaticDiagInfoDescriptionStringTable, ENUM##_desc),
// // clang-format off
// #include "stone/Diag/DiagnosticBasicKind.inc"
// #include "stone/Diag/DiagnosticParseKind.inc"
// // clang-format on
// #undef DIAG
// };

// // Diagnostic classes.
// enum {
//   CLASS_NOTE = 0x01,
//   CLASS_REMARK = 0x02,
//   CLASS_WARNING = 0x03,
//   CLASS_EXTENSION = 0x04,
//   CLASS_ERROR = 0x05
// };

// struct StaticDiagInfoRec {
//   diags::DiagID ID;
//   uint8_t DefaultSeverity : 3;
//   uint8_t Class : 3;
//   uint8_t SFINAE : 2;
//   uint8_t Category : 6;
//   uint8_t WarnNoWerror : 1;
//   uint8_t WarnShowInSystemHeader : 1;
//   uint8_t WarnShowInSystemMacro : 1;

//   uint16_t OptionGroupIndex : 15;
//   uint16_t Deferrable : 1;

//   uint16_t DescriptionLen;

//   unsigned getOptionGroupIndex() const { return OptionGroupIndex; }

//   StringRef getDescription() const {
//     size_t MyIndex = this - &StaticDiagInfo[0];
//     uint32_t StringOffset = StaticDiagInfoDescriptionOffsets[MyIndex];
//     const char *Table =
//         reinterpret_cast<const char *>(&StaticDiagInfoDescriptions);
//     return StringRef(&Table[StringOffset], DescriptionLen);
//   }

//   diags::Flavor getFlavor() const {
//     return Class == CLASS_REMARK ? diags::Flavor::Remark
//                                  : diags::Flavor::WarningOrError;
//   }

//   bool operator<(const StaticDiagInfoRec &RHS) const { return ID < RHS.ID; }
// };

// const StaticDiagInfoRec StaticDiagInfo[] = {
// // clang-format off
// #define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR, \
//              SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY) \
//   { \
//       diags::ENUM, \
//       DEFAULT_SEVERITY, \
//       CLASS, \
//       diags::DiagIDContext::SFINAE, \
//       CATEGORY, \
//       NOWERROR, \
//       SHOWINSYSHEADER, \
//       SHOWINSYSMACRO, \
//       GROUP, \
//         DEFERRABLE, \
//       STR_SIZE(DESC, uint16_t)},
// #include "stone/Diag/DiagnosticBasicKind.inc"
// #include "stone/Diag/DiagnosticParseKind.inc"
// // clang-format on
// #undef DIAG
// };

// } // namespace

// static const unsigned StaticDiagInfoSize = std::size(StaticDiagInfo);

// static const StaticDiagInfoRec *GetDiagInfo(diags::DiagID ID) {
//   const StaticDiagInfoRec *Found = &StaticDiagInfo[(unsigned)ID];
//   // If the diag id doesn't match we found a different diag, abort. This can
//   // happen when this function is called with an ID that points into a hole
//   in
//   // the diagID space.
//   if (Found->ID != ID) {
//     return nullptr;
//   }
//   return Found;
// }

// diags::DiagIDContext::DiagIDContext() {}

// diags::DiagIDContext::~DiagIDContext() {}

// diags::DiagID diags::DiagIDContext::CreateCustomFromFormatString(
//     DiagnosticLevel DiagLevel, llvm::StringRef FormatString) {}

// llvm::StringRef diags::DiagIDContext::GetDescription(diags::DiagID ID) const
// {}

// bool diags::DiagIDContext::IsBuiltinWarningOrExtension(diags::DiagID ID) {}

// bool diags::DiagIDContext::IsDefaultMappingAsError(diags::DiagID ID) {}

// diags::DiagnosticMapping
// diags::DiagIDContext::GetDefaultMapping(diags::DiagID ID) {}

// bool diags::DiagIDContext::IsBuiltinNote(diags::DiagID ID) {}
