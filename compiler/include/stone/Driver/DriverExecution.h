#ifndef STONE_DRIVER_DRIVER_EXECUTION_H
#define STONE_DRIVER_DRIVER_EXECUTION_H

namespace stone {

enum class DriverExecutionKind : uint8_t {
  Compile = 0,
  Backend,
  MergeModule,
  ModuleWrap,
  DynamicLink,
  StaticLink,
  ExecutableLink,
  Assemble,
  First = Compile,
  Last = Assemble
};

class DriverExecution {
public:
	
};

class CompileExecution : public DriverExecution {
public:
};

class DynamicLinkExecution : public DriverExecution {
public:
};

class StaticLinkExecution : public DriverExecution {
public:
};

class ExecutableLinkExecution : public DriverExecution {
public:
};

} // namespace stone
#endif
