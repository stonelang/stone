#ifndef STONE_DIAG_DIAGNOSTIC_PRINTER_H
#define STONE_DIAG_DIAGNOSTIC_PRINTER_H

#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/TextDiagnosticEmitter.h"

#include <memory>

namespace stone {
namespace diags {

class TextDiagnosticPrinter final : public DiagnosticClient {
  std::unique_ptr<TextDiagnosticEmitter> emitter;

public:
};

} // namespace diags

} // namespace stone

#endif