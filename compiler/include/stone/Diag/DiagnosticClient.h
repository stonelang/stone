#ifndef STONE_DIAG_DIAGNOSTIC_CLIENT_H
#define STONE_DIAG_DIAGNOSTIC_CLIENT_H

namespace stone {

namespace diags {

class DiagnosticClient {
public:
  DiagnosticClient();
};

class BlankDiagnosticClient : public DiagnosticClient {
public:
};

/// Diagnostic consumer that forwards diagnostics along to an
/// existing, already-initialized diagnostic consumer.
///
class ForwardingDiagnosticClient : public DiagnosticClient {

public:
};

} // namespace diags

} // namespace stone

#endif