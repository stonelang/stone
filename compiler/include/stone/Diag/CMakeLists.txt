macro(stone_diag_gen component)
  stone_tablegen(${component}Diagnostics.inc
    -gen-stone-diags-defs -stone-component=${component}
    SOURCE DiagnosticEngine.td
    TARGET Stone${component}Diagnostic
)
endmacro(stone_diag_gen)

stone_diag_gen(Basic)
stone_diag_gen(Parse)
