macro(stone_diag_gen component)
  stone_tablegen(Diagnostic${component}Kind.inc
    -gen-stone-diags-defs -stone-component=${component}
    SOURCE DiagnosticEngine.td
    TARGET ClangDiagnostic${component}
)
endmacro(stone_diag_gen)

stone_diag_gen(Basic)
stone_diag_gen(Parse)

 stone_tablegen(AllDiagnosticKind.inc -gen-stone-diags-defs
  SOURCE DiagnosticEngine.td
  TARGET AllClangDiagnosticKind)