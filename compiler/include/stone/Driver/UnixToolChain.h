/*
class Unix : public ToolChain {
public:
  Unix(const Driver &driver, const llvm::Triple &triple);
  ~Unix() = default;
};
*/

/*
class FreeBSD final : public ToolChain {
public:
  FreeBSD(const Driver &driver, const llvm::Triple &triple);
  ~FreeBSD() = default;

public:
  Tool *BuildAssembleTool() override const;
  Tool *BuildLinkTool() override const;
  Tool *BuildStaticLibTool() override const;
  Tool *BuildDynamicLibTool() override const;
  Tool *GetTool(ActionKind modeType) override const;
};
class OpenBSD final : public UnixToolChain {
public:
  OpenBSD(const Driver &driver, const llvm::Triple &triple);
  ~OpenBSD() = default;

public:
  Tool *BuildAssembleTool() override const;
  Tool *BuildLinkTool() override const;
  Tool *BuildStaticLibTool() override const;
  Tool *BuildDynamicLibTool() override const;
  Tool *GetTool(ActionKind modeType) override const;
};
