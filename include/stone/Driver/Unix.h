/*
class UnixToolChain : public ToolChain {
public:
  UnixToolChain(const Driver &driver, const llvm::Triple &triple);
  ~UnixToolChain() = default;
};
*/

/*
class FreeBSDToolChain final : public ToolChain {
public:
  FreeBSDToolChain(const Driver &driver, const llvm::Triple &triple);
  ~FreeBSDToolChain() = default;

public:
  Tool *BuildAssembleTool() override const;
  Tool *BuildLinkTool() override const;
  Tool *BuildStaticLibTool() override const;
  Tool *BuildDynamicLibTool() override const;
  Tool *GetTool(ModeKind modeType) override const;
};
class OpenBSDToolChain final : public UnixToolChain {
public:
  OpenBSDToolChain(const Driver &driver, const llvm::Triple &triple);
  ~OpenBSDToolChain() = default;

public:
  Tool *BuildAssembleTool() override const;
  Tool *BuildLinkTool() override const;
  Tool *BuildStaticLibTool() override const;
  Tool *BuildDynamicLibTool() override const;
  Tool *GetTool(ModeKind modeType) override const;
};
