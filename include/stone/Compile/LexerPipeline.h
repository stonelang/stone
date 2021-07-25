#ifndef STONE_CODEANALYSIS_LEXERPIPELINE_H
#define STONE_CODEANALYSIS_LEXERPIPELINE_H

#include "stone/Basic/PipelineEngine.h"
#include "stone/Basic/Token.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class LexerPipeline : public Pipeline {
public:
  LexerPipeline() : Pipeline(PipelineType::Lex) {}

public:
  llvm::StringRef GetName() override { return "Lex"; }

public:
  virtual void OnTokenCreated(const syn::Token &token) = 0;
};

} // namespace stone
#endif
