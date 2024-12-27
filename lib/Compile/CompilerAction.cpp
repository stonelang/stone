#include "stone/Compile/CompilerInstance.h"

using namespace stone;

CompilerInstance::Action::Action(CompilerInstance &instance)
    : instance(instance) {}
CompilerInstance::Action::~Action() = default;

class PrintHelpAction final : public CompilerInstance::Action {
public:
  PrintHelpAction(CompilerInstance &instance)
      : CompilerInstance::Action(instance) {}

public:
  bool ExecuteAction() {
    assert(GetSelfActionKind() == GetPrimaryActionKind() &&
           "PrintHelpAction has to be the PrimaryAction!");

    instance.GetInvocation().GetCompilerOptions().PrintHelp();
    return true;
  }

  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::PrintHelp;
  }
};
class PrintHelpHiddenAction final : public CompilerInstance::Action {
public:
  PrintHelpHiddenAction(CompilerInstance &instance)
      : CompilerInstance::Action(instance) {}

public:
  bool ExecuteAction() {
    assert(GetSelfActionKind() == GetPrimaryActionKind() &&
           "PrintHelpHiddenAction has to be the PrimaryAction!");

    instance.GetInvocation().GetCompilerOptions().PrintHelp(true);
    return true;
  }

  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::PrintHelpHidden;
  }
};

class PrintVersionAction final : public CompilerInstance::Action {

public:
  PrintVersionAction(CompilerInstance &instance)
      : CompilerInstance::Action(instance) {}

public:
  bool ExecuteAction() override {
    assert(GetSelfActionKind() == GetPrimaryActionKind() &&
           "PrintVersionAction has to be the PrimaryAction!");

    return true;
  }
  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::PrintVersion;
  }
};

class PrintFeatureAction final : public CompilerInstance::Action {
public:
  PrintFeatureAction(CompilerInstance &instance)
      : CompilerInstance::Action(instance) {}

public:
  bool ExecuteAction() override {
    assert(GetSelfActionKind() == GetPrimaryActionKind() &&
           "PrintFeatureAction has to be the PrimaryAction!");

    return true;
  }
  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::PrintFeature;
  }
};