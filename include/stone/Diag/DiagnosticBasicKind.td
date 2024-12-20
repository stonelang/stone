let Component = "Basic" in {

// Substitutions.
def select_constexpr_spec_kind : TextSubstitution<
  "%select{<ERROR>|constexpr|consteval|constinit}0">;

// Basic.

def fatal_too_many_errors
  : Error<"too many errors emitted, stopping now">, DefaultFatal;

}