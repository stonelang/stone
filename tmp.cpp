

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