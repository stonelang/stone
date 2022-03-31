

enum class CompilationMode {

  // N compile job(s) per N files => N^2 parses
  Quadratic,
  /// Multiple compile jobs

  // One compile job per file, with each job having a single primary => N
  // parses
  Flat,

  // One compile job per CPU, identifying an equal-sized "batch" of
  // the module's files as primaries
  CPU,

  /// One compile for the entire module,
  Single,
}

static void
BuildJobs() {

  switch (GetMode()) {
  case ModeKind::Parse:
  case ModeKind::TypeCheck:
    BuildCompileJobs();
    return;
  }

  if (LinkOnly)
    BuildLinkJobONly()
}

int main() { return 0; }





diag::lex_nul_character
diag::lex_invalid_utf8
diag::lex_unterminated_block_comment
diag::lex_comment_start
diag::lex_unary_equal
diag::expected_member_name
diag::extra_whitespace_period
diag::lex_unexpected_block_comment_end
diag::lex_unexpected_block_comment_end
diag::lex_invalid_digit_in_int_literal
diag::lex_expected_binary_exponent_in_hex_float_literal
diag::lex_invalid_digit_in_fp_exponent
diag::lex_expected_digit_in_fp_exponent
diag::lex_invalid_digit_in_int_literal
lex_invalid_u_escape_rbrace
lex_invalid_u_escape
lex_invalid_closing_delimiter
lex_invalid_escape_delimiter
lex_unprintable_ascii_character
lex_nul_character
lex_invalid_escape
lex_unicode_escape_braces
lex_invalid_unicode_scalar
lex_escaped_newline_at_lastline
lex_illegal_multiline_string_end
lex_multiline_string_indent_inconsistent
lex_multiline_string_indent_change_line
lex_illegal_multiline_string_start
lex_unterminated_string
lex_invalid_curly_quote
lex_unterminated_regex
lex_conflict_marker_in_file
lex_invalid_identifier_start_character
lex_nonbreaking_space
lex_invalid_curly_quote
lex_invalid_character
lex_confusable_character
lex_editor_placeholder_in_playground
lex_editor_placeholder
lex_hashbang_not_allowed
