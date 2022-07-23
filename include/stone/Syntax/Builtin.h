#ifndef STONE_SYNTAX_BUILTIN_H
#define STONE_SYNTAX_BUILTIN_H

namespace stone {
namespace syn {
class SyntaxContext;

namespace builtin {

enum BuiltinID {};

struct BuiltinProfile {
  const char *name;
  const char *ty;
  const char *attributes;
  const char *features;
};

} // namespace builtin

class Builtin final {
public:
  Builtin(const Builtin &) = delete;
  void operator=(const Builtin &) = delete;

  void InitType(SyntaxContext &tc);
  void InitTypes(SyntaxContext &tc);

public:
  Builtin() = default;
  ~Builtin();

  void Init(SyntaxContext &tc);
};
} // namespace syn
} // namespace stone
#endif
